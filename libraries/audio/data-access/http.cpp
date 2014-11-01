#include "http.h"

#include <curl/curl.h>

#include <cstdlib>
#include <cstring>

namespace {
  CURLM* sMultiHandle;
}

class HTTP::Impl
{
  struct CURLData
  {
    CURL* handle;
    unsigned char* buffer;
    std::size_t buffer_len;
    std::size_t buffer_pos;
    int still_running;
  };

public:
  Impl(const std::string& aUrl)
    : iUrl(aUrl)
  {
  }

  bool open()
  {
    iCURLData = new CURLData;
    iCURLData->buffer = static_cast<unsigned char*>(malloc(256));

    if(!iCURLData)
      return false;

    iCURLData->handle = curl_easy_init();

    curl_easy_setopt(iCURLData->handle, CURLOPT_URL, iUrl.c_str());
    curl_easy_setopt(iCURLData->handle, CURLOPT_WRITEDATA, iCURLData);
    curl_easy_setopt(iCURLData->handle, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(iCURLData->handle, CURLOPT_WRITEFUNCTION, write_callback);

    if(!sMultiHandle)
      sMultiHandle = curl_multi_init();

    curl_multi_add_handle(sMultiHandle, iCURLData->handle);

    /* lets start the fetch */ 
    curl_multi_perform(sMultiHandle, &iCURLData->still_running);

    if((iCURLData->buffer_pos == 0) && (!iCURLData->still_running)) {
      /* if still_running is 0 now, we should return NULL */ 

      /* make sure the easy handle is not in the multi handle anymore */ 
      curl_multi_remove_handle(sMultiHandle, iCURLData->handle);

      /* cleanup */ 
      curl_easy_cleanup(iCURLData->handle);
 
      delete iCURLData;
      iCURLData = NULL;

      return false;
    }

    return true;
  }

  bool close()
  {
    /* make sure the easy handle is not in the multi handle anymore */ 
    curl_multi_remove_handle(sMultiHandle, iCURLData->handle);

    /* cleanup */ 
    curl_easy_cleanup(iCURLData->handle);

    if(iCURLData->buffer)
      free(iCURLData->buffer);/* free any allocated buffer space */ 

    delete iCURLData;

    return true;
  }

  std::size_t read(unsigned char* aBuffer, std::size_t aByteCount)
  {
    fill_buffer(iCURLData, aByteCount);
 
    /* check if theres data in the buffer - if not fill_buffer()
     * either errored or EOF */ 
    if(!iCURLData->buffer_pos)
      return 0;
 
    /* ensure only available data is considered */ 
    if(iCURLData->buffer_pos < aByteCount)
      aByteCount = iCURLData->buffer_pos;
 
    /* xfer data to caller */ 
    memcpy(aBuffer, iCURLData->buffer, aByteCount);
 
    use_buffer(iCURLData, aByteCount);
 
    return aByteCount;     /* number of items */ 
  }

private:
  static std::size_t write_callback(char* buffer,
                                    std::size_t size,
                                    std::size_t nitems,
                                    void* userp)
  {
    unsigned char *newbuff;
    std::size_t rembuff;

    CURLData *url = static_cast<CURLData *>(userp);
    size *= nitems;

    rembuff=url->buffer_len - url->buffer_pos; /* remaining space in buffer */ 

    if(size > rembuff) {
      /* not enough space in buffer */ 
      newbuff = static_cast<unsigned char*>(realloc(url->buffer, url->buffer_len + (size - rembuff)));
      if(newbuff==NULL) {
        fprintf(stderr,"callback buffer grow failed\n");
        size=rembuff;
      }
      else {
        /* realloc suceeded increase buffer size*/ 
        url->buffer_len+=size - rembuff;
        url->buffer=newbuff;
      }
    }

    memcpy(&url->buffer[url->buffer_pos], buffer, size);
    url->buffer_pos += size;

    return size;
  }

  static int fill_buffer(CURLData* file, size_t want)
  {
    fd_set fdread;
    fd_set fdwrite;
    fd_set fdexcep;
    struct timeval timeout;
    int rc;

    /* only attempt to fill buffer if transactions still running and buffer
     * doesnt exceed required size already
     */ 
    if((!file->still_running) || (file->buffer_pos > want))
      return 0;

    /* attempt to fill buffer */ 
    do {
      int maxfd = -1;
      long curl_timeo = -1;

      FD_ZERO(&fdread);
      FD_ZERO(&fdwrite);
      FD_ZERO(&fdexcep);

      /* set a suitable timeout to fail on */ 
      timeout.tv_sec = 60; /* 1 minute */ 
      timeout.tv_usec = 0;

      curl_multi_timeout(sMultiHandle, &curl_timeo);
      if(curl_timeo >= 0) {
        timeout.tv_sec = curl_timeo / 1000;
        if(timeout.tv_sec > 1)
          timeout.tv_sec = 1;
        else
          timeout.tv_usec = (curl_timeo % 1000) * 1000;
      }

      /* get file descriptors from the transfers */ 
      curl_multi_fdset(sMultiHandle, &fdread, &fdwrite, &fdexcep, &maxfd);
   
      /* In a real-world program you OF COURSE check the return code of the
         function calls.  On success, the value of maxfd is guaranteed to be
         greater or equal than -1.  We call select(maxfd + 1, ...), specially
         in case of (maxfd == -1), we call select(0, ...), which is basically
         equal to sleep. */ 

      rc = select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);

      switch(rc) {
      case -1:
        /* select error */ 
        break;

      case 0:
      default:
        /* timeout or readable/writable sockets */ 
        curl_multi_perform(sMultiHandle, &file->still_running);
        break;
      }
    } while(file->still_running && (file->buffer_pos < want));

    return 1;
  }

  static int use_buffer(CURLData* file, int want)
  {
    /* sort out buffer */ 
    if((file->buffer_pos - want) <=0) {
      /* ditch buffer - write will recreate */ 
      if(file->buffer)
        free(file->buffer);
   
      file->buffer=NULL;
      file->buffer_pos=0;
      file->buffer_len=0;
    }
    else {
      /* move rest down make it available for later */ 
      memmove(file->buffer,
              &file->buffer[want],
              (file->buffer_pos - want));
   
      file->buffer_pos -= want;
    }
    return 0;
  }

private:
  std::string iUrl;
  CURLData* iCURLData;
};

HTTP::HTTP(const std::string& aUrl)
  : iImpl(std::make_shared<Impl>(aUrl))
{
}

HTTP::~HTTP()
{
}

bool HTTP::open()
{
  return iImpl->open();
}

bool HTTP::close()
{
  return iImpl->close();
}

bool HTTP::seekable() const
{
  return false;
}

std::size_t HTTP::read(unsigned char* aBuffer, std::size_t aByteCount)
{
  return iImpl->read(aBuffer, aByteCount);
}

std::size_t HTTP::write(unsigned char* aBuffer, std::size_t aByteCount)
{
  return 0;
}

std::size_t HTTP::seek(long aByteOffset)
{
  return 0;
}

std::size_t HTTP::offset()
{
  return 0;
}

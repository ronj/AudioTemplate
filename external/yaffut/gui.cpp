// Copyright 2006 Rutger E.W. van Beusekom.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "parse.h"

#include <fx.h>

#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <fcntl.h>
#include <signal.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

const int mode = O_RDONLY|O_SYNC;

bool IsSpace(char c)
{
  return ' ' == c;
}

class Gui_output : public Output
{
  std::string current_test;
  std::string current_message;

  FXHiliteStyle style_[2];
  FXText *text_;
  enum colour { plain, pass, fail };
  colour result;
  bool test_name_printed_;

  void set_current_test_(const char* begin, const char* end)
  {
    current_test.assign(begin,end);
  }
  void show_test_(const char*, const char*)
  {
    if(!current_message.empty())
    {
      boost::algorithm::replace_all(current_message, "\n", "\n  ");
      boost::algorithm::trim_right_if(current_message, IsSpace);
      text_->appendStyledText(current_message.c_str (), current_message.size (),
			      result);
      current_message.clear();
    }
    if (!current_test.empty () && !test_name_printed_)
    {
      text_->appendStyledText(current_test.c_str (), current_test.size (), plain);
      text_->appendStyledText(" ", 1, plain);
      test_name_printed_ = true;
    }
  }
  void set_msg_(const char* begin, const char* end)
  {
    current_message.append(begin, end);
  }
  void show_msg_(const char*, const char*)
  {
    if(!current_message.empty())
    {
      text_->appendStyledText(current_message.c_str (), current_message.size (),
			      result);
      current_message.clear();
      result = plain;
    }
  }
  void show_result_(const char* begin, const char* end)
  {
    std::string res(begin, end);
    if(std::string::npos != res.find("[OK]"))
    {
      result = pass;
      ++passed_;
    }
    else
    {
      result = fail;
      ++failed_;
    }
    text_->appendStyledText(begin, end - begin, result);
    test_name_printed_ = false;
  }
public:
  size_t passed_;
  size_t failed_;

  Gui_output(): passed_(0), failed_(0)
  {
  }
  Gui_output (FXText *text)
    : text_ (text)
    , result (plain)
    , passed_(0)
    , failed_(0)
  {
    set_current_test = boost::bind(&Gui_output::set_current_test_, this, _1, _2);
    show_test = boost::bind(&Gui_output::show_test_, this, _1, _2);
    set_msg = boost::bind(&Gui_output::set_msg_, this, _1, _2);
    show_msg = boost::bind(&Gui_output::show_msg_, this, _1, _2);
    show_result = boost::bind(&Gui_output::show_result_, this, _1, _2);
  
    style_[0].normalForeColor = FXRGB(0,200,0);
    style_[0].normalBackColor = FXApp::instance()->getBackColor();
    style_[0].selectForeColor = FXApp::instance()->getSelforeColor();
    style_[0].selectBackColor = FXApp::instance()->getSelbackColor();
    style_[0].hiliteForeColor = FXApp::instance()->getHiliteColor();
    style_[0].hiliteBackColor = FXApp::instance()->getHiliteColor();
    style_[0].activeBackColor = FXApp::instance()->getBackColor();
    style_[0].style = 0;

    style_[1] = style_[0];
    style_[1].normalForeColor = FXRGB(255,0,0);

    text_->setStyled(true);
    text_->setHiliteStyles(style_);
  }
  Gui_output operator = (const Gui_output& go)
  {
    text_ = go.text_;
    result = plain;
    passed_ = 0;
    failed_ = 0;
  
    set_current_test = boost::bind(&Gui_output::set_current_test_, this, _1, _2);
    show_test = boost::bind(&Gui_output::show_test_, this, _1, _2);
    set_msg = boost::bind(&Gui_output::set_msg_, this, _1, _2);
    show_msg = boost::bind(&Gui_output::show_msg_, this, _1, _2);
    show_result = boost::bind(&Gui_output::show_result_, this, _1, _2);
  
    style_[0].normalForeColor = FXRGB(0,200,0);
    style_[0].normalBackColor = FXApp::instance()->getBackColor();
    style_[0].selectForeColor = FXApp::instance()->getSelforeColor();
    style_[0].selectBackColor = FXApp::instance()->getSelbackColor();
    style_[0].hiliteForeColor = FXApp::instance()->getHiliteColor();
    style_[0].hiliteBackColor = FXApp::instance()->getHiliteColor();
    style_[0].activeBackColor = FXApp::instance()->getBackColor();
    style_[0].style = 0;

    style_[1] = style_[0];
    style_[1].normalForeColor = FXRGB(255,0,0);

    text_->setStyled(true);
    text_->setHiliteStyles(style_);

    return *this;
  }
};

class gui: public FXMainWindow
{
  FXDECLARE(gui);
    
  std::string m_TestExec;

  FXProgressBar* m_ProgressBar;

  FXStatusBar* m_Status;

  FXLabel* m_Runs;
  FXLabel* m_Success;
  FXLabel* m_Fail;

  FXText* m_Message;
  
  FXButton* m_Run;

  FXTreeList* m_List;

  FXFont* m_MessageFont;
  FXFont* m_StatusFont;
  FXFont* m_ListFont;

  size_t m_Total;
  std::string m_Result;  
  bool m_Stopped;
  std::vector<char> m_Buffer;

  Gui_output output;
    
  boost::mutex m_mutex;
  boost::thread* m_TestThread;
  boost::thread* m_ReadThread;
protected:
  gui();
public:
  enum
  {
    ID_RUN = FXMainWindow::ID_LAST,
    ID_STOP,
    ID_IDLE,
    ID_LOAD,
  };
    
  gui(FXApp& app, const std::string& test_exec);
  ~gui();
  
  virtual void create();
  
  long OnRun(FXObject*, FXSelector, void*);
  long OnStop(FXObject*, FXSelector, void*);
  long OnIdle(FXObject*, FXSelector, void*);  
  long OnLoad(FXObject*, FXSelector, void*);  
private:
  void LeftFrame(FXVerticalFrame* leftvframe);
  void RightFrame(FXVerticalFrame* rightvframe);

  void SetTimeout();

  void TestThread(const std::vector<std::string>& tests);
  void ReadThread();

  std::vector<std::string> ListTests();
};

const size_t DefaultWidth = 800;
const size_t DefaultHeight = 600;

const int LAYOUT_FILL_XY = LAYOUT_FILL_X|LAYOUT_FILL_Y;

FXDEFMAP(gui) guiMap[] = 
{
  FXMAPFUNC(SEL_COMMAND, gui::ID_RUN,  gui::OnRun),
  FXMAPFUNC(SEL_COMMAND, gui::ID_STOP, gui::OnStop),
  FXMAPFUNC(SEL_TIMEOUT, gui::ID_IDLE, gui::OnIdle),
  FXMAPFUNC(SEL_COMMAND, gui::ID_LOAD, gui::OnLoad),
};

FXIMPLEMENT(gui, FXMainWindow, guiMap, ARRAYNUMBER(guiMap));
  
gui::gui(){}

gui::gui(FXApp& app, const std::string& test_exec)
  : FXMainWindow(&app,"gui",NULL,NULL,DECOR_ALL,0,0,DefaultWidth,DefaultHeight)
  , m_TestExec(test_exec)
  , m_Total(0)
  , m_Result()
  , m_Stopped(true)
  , m_Buffer(1024)
  , m_TestThread(0)
  , m_ReadThread(0)
{
  remove(".test-pipe");
  remove(".load-pipe");
  mkfifo(".test-pipe", 0600);
  mkfifo(".load-pipe", 0600);

  FXVerticalFrame* vframe =
    new FXVerticalFrame(this, LAYOUT_FILL_XY);

  FXHorizontalFrame* hframe =
    new FXHorizontalFrame(vframe, LAYOUT_FILL_X);

  FXVerticalFrame* leftvframe = 
    new FXVerticalFrame(hframe, LAYOUT_FILL_XY);
  
  FXVerticalFrame* rightvframe = 
    new FXVerticalFrame(hframe, LAYOUT_FILL_Y);  

  m_MessageFont = new FXFont(getApp(), "", 12);
  m_StatusFont = new FXFont(getApp(), "", 12, FONTWEIGHT_BOLD);
  m_ListFont = new FXFont(getApp(), "", 12, FONTWEIGHT_NORMAL, FONTSLANT_ITALIC);

  LeftFrame(leftvframe);

  hframe = new FXHorizontalFrame(vframe);
  
  (new FXLabel(hframe,"Executed:",0,LAYOUT_CENTER_X))->setFont(m_StatusFont);
  (m_Runs = new FXLabel(hframe,"0",0,LAYOUT_CENTER_X))->setFont(m_StatusFont);
  (new FXLabel(hframe,"Succeeded:",0,LAYOUT_CENTER_X))->setFont(m_StatusFont);
  (m_Success = new FXLabel(hframe,"0",0,LAYOUT_CENTER_X))->setFont(m_StatusFont);
  (new FXLabel(hframe,"Failures:",0,LAYOUT_CENTER_X))->setFont(m_StatusFont);
  (m_Fail = new FXLabel(hframe,"0",0,LAYOUT_CENTER_X))->setFont(m_StatusFont);
  (new FXLabel(hframe," ",0,LAYOUT_CENTER_X))->setFont(m_StatusFont);

  new FXHorizontalSeparator(vframe);

  new FXLabel(vframe, "failures");
  FXHorizontalFrame* sunken = new FXHorizontalFrame(vframe, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_XY,
                                                    0,0,0,0,0,0,0,0,0,0);
  m_Message = new FXText(sunken,0,0,TEXT_READONLY|LAYOUT_FILL_XY);
  m_Message->setCursorColor(FXRGB(255,255,255));
  m_Message->setFont(m_StatusFont);

  m_Status = new FXStatusBar(vframe, LAYOUT_FILL_X,0,0,0,0,0,0,0,0);
  m_Status->getStatusLine()->setNormalText("idle");

  RightFrame(rightvframe);    

  SetTimeout();
}

gui::~gui()
{
  OnStop(0,0,0);

  if(m_TestThread)
  {
    m_TestThread->join();
    delete m_TestThread;
  }
  if(m_ReadThread)
  {
    m_ReadThread->join();
    delete m_ReadThread;
  }
  remove(".load-pipe");
  remove(".test-pipe");
}

void gui::SetTimeout()
{
  getApp()->addTimeout(this, gui::ID_IDLE, 100);
}

void gui::RightFrame(FXVerticalFrame* rightvframe)
{
  const int width = 75;
  const int height = 25;
  m_Run = new FXButton(rightvframe,"Run",0,this,ID_RUN,
                       LAYOUT_FILL_X|BUTTON_NORMAL|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT,
                       0,0,width,height);
  new FXButton(rightvframe,"Stop",0,this,ID_STOP,
               LAYOUT_FILL_X|BUTTON_NORMAL|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT,
               0,0,width,height);

  new FXButton(rightvframe,"Reload",0,this, ID_LOAD,
               LAYOUT_FILL_X|BUTTON_NORMAL|LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT,0,0,width, height);

  new FXButton(rightvframe,"Quit",0,getApp(),FXApp::ID_QUIT,
               LAYOUT_FILL_X|LAYOUT_BOTTOM|BUTTON_NORMAL|
               LAYOUT_FIX_WIDTH|LAYOUT_FIX_HEIGHT,
               0,0,width,height);
}
void gui::LeftFrame(FXVerticalFrame* leftvframe)
{
  new FXLabel(leftvframe, "select test(s):");

  FXVerticalFrame* vframe =
    new FXVerticalFrame(leftvframe, FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|LAYOUT_FILL_Y,
                        0,0,0,0,0,0,0,0,0,0);
  m_List = 
    new FXTreeList(vframe,0,0,
                   FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_X|TREELIST_SHOWS_LINES|
                   TREELIST_SHOWS_BOXES|TREELIST_ROOT_BOXES|TREELIST_EXTENDEDSELECT|TREELIST_MULTIPLESELECT);
  
  m_List->setFont(m_ListFont);
  m_List->setNumVisible(10);

  OnLoad(0,0,0);
  
  new FXLabel(leftvframe, "progress:");
  m_ProgressBar = 
    new FXProgressBar(leftvframe,0,0,LAYOUT_FILL_X|PROGRESSBAR_NORMAL);
  
  m_ProgressBar->setBarSize(25);
  m_ProgressBar->showNumber();
}
void gui::create()
{
  FXMainWindow::create();
  show(PLACEMENT_SCREEN);
}
long gui::OnIdle(FXObject*, FXSelector, void*)
{
  boost::mutex::scoped_lock lock(m_mutex);
  if(m_Stopped)
  {
    m_Run->enable();
    m_Status->getStatusLine()->setNormalText("idle");

    lock.unlock();
    if(m_TestThread)
    {
      m_TestThread->join();
      delete m_TestThread;
      m_TestThread = 0;
    }
    if(m_ReadThread)
    {
      m_ReadThread->join();
      delete m_ReadThread;
      m_ReadThread = 0;
    }
  }
  if(!m_Result.empty())
  {
    test_parser tp(output);

    boost::spirit::parse_info<> info = boost::spirit::parse(m_Result.c_str(), tp);
    if (info.full)
    {
      m_Result.clear();
    }
    else if(info.hit)
    {
      m_Result.erase(0, info.length );
    }
  }

  if(output.failed_)
  {
    m_ProgressBar->setBarColor(FXRGB(255,0,0));
  }
  m_ProgressBar->setProgress(output.passed_ + output.failed_);

  m_Runs->setText(boost::lexical_cast<std::string>(output.passed_ + output.failed_).c_str());
  m_Success->setText(boost::lexical_cast<std::string>(output.passed_).c_str());
  m_Fail->setText(boost::lexical_cast<std::string>(output.failed_).c_str());

  SetTimeout();
   
  return 1;
}
long gui::OnRun(FXObject*, FXSelector, void*)
{
  if (!m_List->getFirstItem ())
  {
    OnLoad (0, 0, 0);
  }
  boost::mutex::scoped_lock lock(m_mutex);
  if(m_Stopped)
  {
    m_Stopped = false;

    m_Total = 0;
    m_Message->setText("");

    output = Gui_output(m_Message);

    m_ProgressBar->setProgress(0);
    m_ProgressBar->setBarColor(FXRGB(0,255,0));

    std::vector<std::string> tests;
    std::vector<std::string> all;
    std::string test;
    FXTreeItem* item = m_List->getFirstItem();
    bool selectAll = false;
    if(item->isSelected())
    {
      selectAll = true;
    }
    FXTreeItem* suite = item->getBelow();
    item = item->getBelow();
    bool useChildren = false or selectAll;
    do
    {
      if(item->getNumChildren())
      {
        suite = item;
        if(item->isSelected())
        {
          useChildren = item->getNumChildren() or selectAll;
        }
        else
        {
          useChildren = false or selectAll;
        }
      }
      else
      {
        if(item->getParent() == suite)
        {
          test.append(m_List->getItemText(suite).text()).append("::");
        }
        else
        {
          test.append("::");
        }
        test.append(m_List->getItemText(item).text()).append(" ");
        if(useChildren or item->isSelected())
        {
          tests.push_back(test);
        }
        all.push_back(test);
        test.clear();
      }
      item = item->getBelow();
    }
    while(item);

    if(tests.empty())
    {
      tests = all;
    }

    m_Run->disable();
    
    if(!m_ReadThread)
    {
      m_ReadThread = new boost::thread(boost::bind(&gui::ReadThread, this));
    }
    if(!m_TestThread)
    {
      m_Total = tests.size();
      m_ProgressBar->setTotal(m_Total);
      m_TestThread = 
        new boost::thread(boost::bind(&gui::TestThread, this, tests));
      m_Status->getStatusLine()->setNormalText("running");
    }
  }
  return 1;
}
long gui::OnStop(FXObject*, FXSelector, void*)
{
  boost::mutex::scoped_lock lock(m_mutex);
  if(output.pid_)
  {
    kill(output.pid_, 13);
  }
  m_Stopped = true;
  m_Result.clear();
  return 1;
}
long gui::OnLoad(FXObject*, FXSelector, void*)
{
  std::string testName;

  std::vector<std::string> names(ListTests());

  m_List->clearItems();

  FXTreeItem* all = m_List->appendItem(0, names[0].c_str());
  FXTreeItem* par = 0;

  for(size_t i = 1; i < names.size(); ++i)
  {
    std::string::size_type pos = names[i].find("::");

    if(testName == names[i].substr(0, pos))
    {
      m_List->appendItem(par, names[i].substr(pos + 2, std::string::npos).c_str());
    }
    else
    {
      testName = names[i].substr(0, pos);
      par = m_List->appendItem(all, testName.c_str());
      m_List->expandTree(par);
      if(pos != std::string::npos)
      {
        m_List->appendItem(par, names[i].substr(pos + 2, std::string::npos).c_str());
      }
    }
  }
  m_List->expandTree(all);

  return 1;
}
void gui::TestThread(const std::vector<std::string>& tests)
{
  std::ostringstream os;
  os << m_TestExec << ' ';
  for(size_t i = 0; i < tests.size(); ++i)
  {
    os << tests[i] << ' ';
  }
  os << "> .test-pipe";

  system(os.str().c_str());

  boost::mutex::scoped_lock lock(m_mutex);
  m_Stopped = true;
}
void gui::ReadThread()
{
  int fd = ::open(".test-pipe", mode);
  assert(-1 != fd);

  boost::mutex::scoped_lock lock(m_mutex);
  int size = 0;
  do
  {
    lock.unlock();
    size = read(fd, &m_Buffer[0], m_Buffer.size() - 1);
    lock.lock();

    if(size > 0)
    {
      m_Buffer[size] = 0;

      m_Result.append(&m_Buffer[0]);
    }
  }
  while(size > 0);

  m_Stopped = true;  
  ::close(fd);
}
std::vector<std::string> gui::ListTests()
{
  system((m_TestExec + " -l 1> .load-pipe 2>/dev/null&").c_str());
  int fd = ::open(".load-pipe", mode);

  assert(-1 != fd);
  std::string test;
  int size;
  do
  {
    size = read(fd, &m_Buffer[0], m_Buffer.size() - 1);
    if(size > 0)
    {
      m_Buffer[size] = 0;
      test.append(&m_Buffer[0], size);
    }
  }
  while(size > 0);

  std::vector<std::string> names;
  
  std::istringstream is(test);
  is >> test;
  while(is)
  {
    is >> test;
    if(is)
    {
      names.push_back(test);
    }
  }
  ::close(fd);

  names.insert(names.begin(), "All");

  return names;
}

void help()
{
  std::clog << "Usage: gui TEST-PROGRAM [-geometry X Y]" << std::endl;
}

int main(int argc, char* argv[])
{
  try
  {
    if(argc < 2 || argv[1] == std::string ("-h")
       || argv[1] == std::string ("--help")
       || *argv[1] == '-')
    {
      help();
      exit(2);
    }

    FXApp app;

    gui* g = new gui(app, argv[1]);

    for(int i = 2; i < argc; ++i)
    {
      if(0 == strcmp(argv[i],"-geometry") && argc != i + 1)
      {
        std::istringstream os(argv[i+1]);
        size_t W = DefaultWidth;
        size_t H = DefaultHeight;
        char x = 0;
        os >> W >> x >> H;
        g->resize(W, H);
      }
    }    
    
    app.init(argc, argv);
    app.create();

    return app.run();
  }
  catch(const std::exception& e)
  {
    std::clog << e.what() << std::endl;
  }
  return 1;
}

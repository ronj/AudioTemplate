#ifndef INVENT_URI_H
#define INVENT_URI_H

#ifdef _MSC_VER
#	pragma warning(disable:4786)
#	pragma warning(disable:4996)
#endif

#include <algorithm>
#include <vector>
#include <string>
#include <exception>

class SyntaxException : public std::exception
{
public:
	SyntaxException(const std::string& aError)
		: m_Message(aError)
	{
	}

	SyntaxException(const std::string& aError, const std::string& aInput)
		: m_Message("Syntax error occured while parsing: " + aInput + ". Error: " + aError)
	{
	}

	const char* what() const throw()
	{
		return m_Message.c_str();
	}

private:
	const std::string m_Message;
};

class URI {
public:
	/// Creates an empty URI.
	URI();

	/// Parses an URI from the given string. Throws a
	/// SyntaxException if the uri is not valid.
	explicit URI(const std::string& uri);

	/// Parses an URI from the given string. Throws a
	/// SyntaxException if the uri is not valid.
	explicit URI(const char* uri);

	/// Creates an URI from its parts.
	URI(const std::string& scheme, const std::string& pathEtc);

	/// Creates an URI from its parts.
	URI(const std::string& scheme, const std::string& authority, const std::string& pathEtc);

	/// Creates an URI from its parts.
	URI(const std::string& scheme, const std::string& authority, const std::string& path, const std::string& query);

	/// Creates an URI from its parts.
	URI(const std::string& scheme, const std::string& authority, const std::string& path, const std::string& query, const std::string& fragment);

	/// Creates an URI from a base URI and a relative URI, according to
	/// the algorithm in section 5.2 of RFC 3986.
	URI(const URI& baseURI, const std::string& relativeURI);

	/// Parses and assigns an URI from the given string. Throws a
	/// SyntaxException if the uri is not valid.
	URI& operator = (const std::string& uri);

	/// Parses and assigns an URI from the given string. Throws a
	/// SyntaxException if the uri is not valid.
	URI& operator = (const char* uri);

	/// Swaps the URI with another one.
	void swap(URI& uri);

	/// Clears all parts of the URI.
	void clear();

	/// Returns a string representation of the URI.
	///
	/// Characters in the path, query and fragment parts will be
	/// percent-encoded as necessary.
	std::string toString() const;

	/// Returns the scheme part of the URI.
	const std::string& getScheme() const;

	/// Sets the scheme part of the URI. The given scheme
	/// is converted to lower-case.
	///
	/// A list of registered URI schemes can be found
	/// at <http://www.iana.org/assignments/uri-schemes>.
	void setScheme(const std::string& scheme);

	/// Returns the user-info part of the URI.
	const std::string& getUserInfo() const;

	/// Sets the user-info part of the URI.
	void setUserInfo(const std::string& userInfo);

	/// Returns the host part of the URI.
	const std::string& getHost() const;

	/// Sets the host part of the URI.
	void setHost(const std::string& host);

	/// Returns the port number part of the URI.
	///
	/// If no port number (0) has been specified, the
	/// well-known port number (e.g., 80 for http) for
	/// the given scheme is returned if it is known.
	/// Otherwise, 0 is returned.
	unsigned short getPort() const;

	/// Sets the port number part of the URI.
	void setPort(unsigned short port);

	/// Returns the authority part (userInfo, host and port)
	/// of the URI.
	///
	/// If the port number is a well-known port
	/// number for the given scheme (e.g., 80 for http), it
	/// is not included in the authority.
	std::string getAuthority() const;

	/// Parses the given authority part for the URI and sets
	/// the user-info, host, port components accordingly.
	void setAuthority(const std::string& authority);

	/// Returns the path part of the URI.
	const std::string& getPath() const;

	/// Sets the path part of the URI.
	void setPath(const std::string& path);

	/// Returns the query part of the URI.
	std::string getQuery() const;

	/// Sets the query part of the URI.
	void setQuery(const std::string& query);

	/// Returns the unencoded query part of the URI.
	const std::string& getRawQuery() const;

	/// Sets the query part of the URI.
	void setRawQuery(const std::string& query);

	/// Returns the fragment part of the URI.
	const std::string& getFragment() const;

	/// Sets the fragment part of the URI.
	void setFragment(const std::string& fragment);

	/// Sets the path, query and fragment parts of the URI.
	void setPathEtc(const std::string& pathEtc);

	/// Returns the path, query and fragment parts of the URI.
	std::string getPathEtc() const;

	// Returns the path and query parts of the URI.
	std::string getPathAndQuery() const;

	/// Resolves the given relative URI against the base URI.
	/// See section 5.2 of RFC 3986 for the algorithm used.
	void resolve(const std::string& relativeURI);

	/// Resolves the given relative URI against the base URI.
	/// See section 5.2 of RFC 3986 for the algorithm used.
	void resolve(const URI& relativeURI);

	/// Returns true if the URI is a relative reference, false otherwise.
	///
	/// A relative reference does not contain a scheme identifier.
	/// Relative references are usually resolved against an absolute
	/// base reference.
	bool isRelative() const;

	/// Returns true if the URI is empty, false otherwise.
	bool empty() const;

	/// Returns true if both URIs are identical, false otherwise.
	///
	/// Two URIs are identical if their scheme, authority,
	/// path, query and fragment part are identical.
	bool operator == (const URI& uri) const;

	/// Parses the given URI and returns true if both URIs are identical,
	/// false otherwise.
	bool operator == (const std::string& uri) const;

	/// Returns true if both URIs are identical, false otherwise.
	bool operator != (const URI& uri) const;

	/// Parses the given URI and returns true if both URIs are identical,
	/// false otherwise.
	bool operator != (const std::string& uri) const;

	/// Normalizes the URI by removing all but leading . and .. segments from the path.
	///
	/// If the first path segment in a relative path contains a colon (:),
	/// such as in a Windows path containing a drive letter, a dot segment (./)
	/// is prepended in accordance with section 3.3 of RFC 3986.
	void normalize();

	/// Places the single path segments (delimited by slashes) into the
	/// given vector.
	void getPathSegments(std::vector<std::string>& segments);

	/// URI-encodes the given string by escaping reserved and non-ASCII
	/// characters. The encoded string is appended to encodedStr.
	static void encode(const std::string& str, const std::string& reserved, std::string& encodedStr);

	/// URI-decodes the given string by replacing percent-encoded
	/// characters with the actual character. The decoded string
	/// is appended to decodedStr.
	static void decode(const std::string& str, std::string& decodedStr);

protected:

	/// Returns true if both uri's are equivalent.
	///
	bool equals(const URI& uri) const;

	/// Returns true if the URI's port number is a well-known one
	/// (for example, 80, if the scheme is http).
	bool isWellKnownPort() const;

	/// Returns the well-known port number for the URI's scheme,
	/// or 0 if the port number is not known.
	unsigned short getWellKnownPort() const;

	/// Parses and assigns an URI from the given string. Throws a
	/// SyntaxException if the uri is not valid.
	void parse(const std::string& uri);

	/// Parses and sets the user-info, host and port from the given data.
	///
	void parseAuthority(std::string::const_iterator& it, const std::string::const_iterator& end);

	/// Parses and sets the host and port from the given data.
	///
	void parseHostAndPort(std::string::const_iterator& it, const std::string::const_iterator& end);

	/// Parses and sets the path from the given data.
	///
	void parsePath(std::string::const_iterator& it, const std::string::const_iterator& end);

	/// Parses and sets the path, query and fragment from the given data.
	///
	void parsePathEtc(std::string::const_iterator& it, const std::string::const_iterator& end);

	/// Parses and sets the query from the given data.
	///
	void parseQuery(std::string::const_iterator& it, const std::string::const_iterator& end);

	/// Parses and sets the fragment from the given data.
	///
	void parseFragment(std::string::const_iterator& it, const std::string::const_iterator& end);

	/// Appends a path to the URI's path.
	///
	void mergePath(const std::string& path);

	/// Removes all dot segments from the path.
	///
	void removeDotSegments(bool removeLeading = true);

	/// Places the single path segments (delimited by slashes) into the
	/// given vector.
	static void getPathSegments(const std::string& path, std::vector<std::string>& segments);

	/// Builds the path from the given segments.
	///
	void buildPath(const std::vector<std::string>& segments, bool leadingSlash, bool trailingSlash);

	static const std::string RESERVED_PATH;
	static const std::string RESERVED_QUERY;
	static const std::string RESERVED_FRAGMENT;
	static const std::string ILLEGAL;

private:
	// Replaces all characters in str with their lower-case counterparts.
	template <class S>
	S& toLower(S& str){
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		return str;
	}

private:
	std::string    _scheme;
	std::string    _userInfo;
	std::string    _host;
	unsigned short _port;
	std::string    _path;
	std::string    _query;
	std::string    _fragment;
};

//
// inlines
//
inline const std::string& URI::getScheme() const
{
	return _scheme;
}


inline const std::string& URI::getUserInfo() const
{
	return _userInfo;
}


inline const std::string& URI::getHost() const
{
	return _host;
}


inline const std::string& URI::getPath() const
{
	return _path;
}


inline const std::string& URI::getRawQuery() const
{
	return _query;
}


inline const std::string& URI::getFragment() const
{
	return _fragment;
}


inline void swap(URI& u1, URI& u2)
{
	u1.swap(u2);
}

#endif // !INVENT_URI_H

ngx_http_auth_filter_module
===========================

NginX WWW-Authorize filter module.

Rationale
---------

Whenever you want to use NginX as a reverse proxy in front of a Microsoft
IIS server, you will get into trouble if the web server is configured to
authorize the accessed resource using one of its proprietary protocol.

"Negotiate" is used between IIS and Internet Explorer to encapsulate
windows authentication. This allow Windows credentials to be passed on
transparently to the web server.

"NTLM" is used between IIS and compatible browsers.

The problem is that those protocols do not play well in a reverse-proxy
environment. This is the case not only for NginX, but for any stateless
reverse proxy.

This is explained in the Microsoft knowledge base article available at :
[https://support.microsoft.com/kb/198116](https://support.microsoft.com/kb/198116)

If you are faced with en ever-repeating credential request dialog on your
browser, you are most probably victim of this situation.

What this module does
---------------------

It simply filters out the offending "Negotiate" and "NTLM" authorization
headers in any response received from the IIS back-end web server.

As a consequence, the web browser is never aware of the availability of the
problem-prone protocols and don't use them.

Naturally, an alternate authorization mechanism ("Basic" or "Digest") needs
to be configured on the IIS server in order to serve the resources properly.

Build
-----

cd to NGINX source directory & run this:

	./configure --add-module=/path/to/ngx_http_auth_filter_module
	make
	make install

Configure
---------

This module is designed to be activated from the "location" directive in
your configuration.

THe presence of the "filter_auth" directive activates the module for this
location.

	server {
	  listen       80;
	
	  location / {
	    proxy_pass                   http://127.0.0.1:8080/;
	    proxy_pass_header            Authorization;
	    proxy_pass_request_headers   on;
	    proxy_set_header     Host            $host;
	    proxy_set_header     X-Real-IP       $remote_addr;
	    proxy_set_header     X-Forwarded-For $proxy_add_x_forwarded_for;
	    filter_auth;
	  }
	
	}


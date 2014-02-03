
/*
 * Author: Jean-Francois Stenuit
 * Licensed under the terms of the GNU General Public License v3.0
 *
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


char *ngx_http_auth_filter_configure(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_auth_filter_init(ngx_conf_t *cf);

/* Module configuration structure */
typedef struct {
    ngx_flag_t  create_full_put_path;
} ngx_http_auth_filter_loc_conf_t;

/* Module directives */
static ngx_command_t  ngx_http_auth_filter_commands[] = {

    { ngx_string("filter_auth"),
      NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
      ngx_http_auth_filter_configure,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

      ngx_null_command
};

/* Module context */
static ngx_http_module_t  ngx_http_auth_filter_module_ctx = {
    NULL,                                  /* preconfiguration */
    ngx_http_auth_filter_init,             /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    NULL,                                  /* create location configuration */
    NULL,                                  /* merge location configuration */
};

/* Module definition */
ngx_module_t  ngx_http_auth_filter_module = {
    NGX_MODULE_V1,
    &ngx_http_auth_filter_module_ctx,  /* module context */
    ngx_http_auth_filter_commands,         /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};

static ngx_http_output_header_filter_pt  ngx_http_next_header_filter;

static ngx_int_t
ngx_http_auth_filter(ngx_http_request_t *r)
{
	/* The actual header filtering code */

	ngx_uint_t                 i;
	ngx_list_part_t           *part;
	ngx_table_elt_t           *header;
    
   ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0, "auth header filter");

	part = &r->headers_out.headers.part;
	header = part->elts;
	for (i = 0 ; /* void */ ; i++) {
		if (i >= part->nelts) {
			if (part->next == NULL) { break; }
			part = part->next;
			header = part->elts;
			i = 0;
		}
		if ((header[i].key.len == 16) && (ngx_strncmp(header[i].lowcase_key, "www-authenticate", 16) == 0))
		{
			if (((header[i].value.len == 4) && (ngx_strncmp(header[i].value.data, "NTLM", 4) == 0))
			 || ((header[i].value.len == 16) && (ngx_strncmp(header[i].value.data, "Negotiate", 4) == 0)))
			{
				/* Set the hash to zero effectively prevents sending the header */
				header[i].hash = 0;	
			}
		}
	}

   return ngx_http_next_header_filter(r);
}

static ngx_int_t
ngx_http_auth_filter_init(ngx_conf_t *cf)
{
	/* Filter initialisation : insert my filter into the filter chain */
    ngx_http_next_header_filter = ngx_http_top_header_filter;
    ngx_http_top_header_filter = ngx_http_auth_filter;

    return NGX_OK;
}

char *ngx_http_auth_filter_configure(ngx_conf_t *cf, ngx_command_t *cmd, void *conf) {
	/* Config initialisation */
	
	return (char *)NULL;
}
    
/**************************************************************************************/ 


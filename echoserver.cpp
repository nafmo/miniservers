#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>

#define BUFLEN 1024

//#define pf(A,B) { FILE *f = fopen("/home/peter/src/simple/log.txt", "a"); if (f) {fprintf(f, A, B); fclose(f); }}
#define pf(A,B)

struct buf
{
	char data[BUFLEN];
	int len;
	struct buf *next;

	buf() { next = NULL; len = 0; };
	~buf() { delete next; };
	
};


int main()
{
	buf *query = NULL, *head, *data = NULL, *body = NULL, *prev;
	int totlen = 0, totbody = 0;
	int c = 0, oc = 0;
	bool stay = true;

	head = query = new buf;
	prev = NULL;

	while (stay && EOF != (c = fgetc(stdin)))
	{
		if ('\n' == c && '\n' == oc)
		{
			stay = false;
			query->len --;
			if (0 == query->len)
			{
				prev->next = NULL;
				delete query;
				query = prev;
			}

			if (0 != query->len)
			{
				query->len --;
			}
			break;
		}

		if (BUFLEN == query->len)
		{
			query->next = new buf;
			prev = query;
			query = query->next;
		}

		query->data[query->len ++] = c;
		totlen ++;
		if ('\r' != c)
			oc = c;
	}

	char *p = strchr(head->data, ' ');
	char *method = NULL;
	if (p)
	{
		method = new char[p - head->data + 1];
		strncpy(method, head->data, p - head->data);
		method[p - head->data] = 0;
	}

	if (0 == strncmp(head->data, "POST ", 5))
	{
pf("POST: totlen=%d\n",totlen);
		char *header = new char[totlen + 1];

		buf *trav = head;
		int ix = 0;
		while (trav)
		{
			memcpy(header + ix, trav->data, trav->len);
			trav = trav->next;
		}
		header[totlen] = 0;
		char *p = strcasestr(header, "Content-Length: ");
pf("POST: HEADER: %s\n", p);

		if (p)
		{
			int bytes;
			sscanf(p + 16, "%d", &bytes);
pf("POST: Content-Length: %d\n",bytes);

			data = body = new buf;
			while (bytes > 0 && EOF != (c = fgetc(stdin)))
			{
				if (BUFLEN == data->len)
				{
					data->next = new buf;
					query = data->next;
				}

				data->data[data->len ++] = c;
				totbody ++;
				bytes --;
pf("%c", (char)c);
			}
		}

		delete[] header;
	}
	fclose(stdin);

	fputs("HTTP/1.0 200 Goddag, goddag\nContent-type: text/html\n\n", stdout);
	printf("<html>\n<head><title>Echo server</title></head>\n"
	       "<!-- Tittut! -->\n"
	       "<body>\n<h1>Echo server results for %s</h1>\n"
	       "<h2>Summary</h2>\n"
	       "<ul>\n"
	       "<li>Total number of header bytes received: %d\n",
	       method, totlen);

	if (0 == strncasecmp(method, "POST", 4))
	{
		printf("<li>Total number of body bytes received: %d\n", totbody);
	}

	fputs("</ul>\n"
	      "<h2>Request headers</h2>\n"
	      "<ul>\n<li>", stdout);

	query = head;
	while (query)
	{
		for (int i = 0; i < query->len; i ++)
		{
			char c = query->data[i];
			if ('\n' == c)
			{
				fputs("\n<li>", stdout);
			}
			else
			{
				switch (c)
				{
					case '&': fputs("&amp;", stdout); break;
					case '<': fputs("&lt;",  stdout); break;
					case '>': fputs("&gt;",  stdout); break;
					default:  fputc(c, stdout);       break;
				}
			}
		}

		query = query->next;
	}
	puts("\n</ul>");

	if (head->data && 0 == strcmp(method, "GET"))
	{
		bool stay = true;

		fputs("<h2>Decoded GET request</h2>\n<pre>", stdout);
		for (int i = 0; i < head->len; i ++)
		{
			char c = head->data[i];
			if ('\n' == c || 0 == c)
			{
				break;
			}
			else
			{
				if ('%' == c && i + 3 < head->len)
				{
					char hex[3] = { 0,0,0 };
					hex[0] = head->data[++ i];
					hex[1] = head->data[++ i];
					int x;
					sscanf(hex, "%x", &x);
					c = char(x);
				}

				if ('<' == c || '>' == c || '&' == c)
				{
					printf("&#%d;", (int) c);
				}
				else if (0 == c)
				{
					fputs("<span style='background: #ffc'>nul</span>", stdout);
				}
				else
				{
					fputc(c, stdout);
				}
			}
		}
		fputs("</pre>\n", stdout);
	}

	delete head;

	if (body)
	{
		fputs("<h2>Request body</h2>\n<pre>", stdout);

		data = body;
		while (data)
		{
			for (int i = 0; i < data->len; i ++)
			{
				char c = data->data[i];

				switch (c)
				{
					case '&': fputs("&amp;", stdout); break;
					case '<': fputs("&lt;",  stdout); break;
					case '>': fputs("&gt;",  stdout); break;
					default:  fputc(c, stdout);       break;
				}
			}

			data = data->next;
		}
		fputs("</pre>\n", stdout);

		delete body;
	}

	puts("</body>\n</html>\n");

	delete method;

	return 0;
}

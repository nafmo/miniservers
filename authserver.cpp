#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>

#define BUFLEN 32768

void fputsafe(int c)
{
	switch (c)
	{
		case 0:
			fputs("<b>&lt;nul&gt;</b>", stdout);
			break;

		case '<':
			fputs("&lt;", stdout);
			break;

		case '>':
			fputs("&gt;", stdout);
			break;

		case '&':
			fputs("&amp;", stdout);
			break;

		default:
			fputc(c, stdout);
	}
}

int main()
{
	char *useragent = NULL;
	char *auth = NULL;
	char buffer[BUFLEN] = {0};

	while (NULL != fgets(buffer, BUFLEN, stdin))
	{
		if ('\r' == buffer[0] || '\n' == buffer[0])
			break;

		if (0 == strncasecmp(buffer, "User-Agent", 10))
		{
			useragent = strdup(buffer);
		}
		else if (0 == strncasecmp(buffer, "Authorization", 13))
		{
			auth = strdup(buffer);
		}
	}

	fclose(stdin);

	if (!auth)
	{
		fputs("HTTP/1.0 401 Vem fan är du?\nContent-type: text/html\n"
		      "WWW-Authenticate: Basic realm=\"Vem är du?\"\n\n", stdout);
		fputs("<h1>Authorization required</h1>\n", stdout);
		if (useragent) fputs(useragent, stdout);
	}
	else
	{
		fputs("HTTP/1.0 200 Välkommen!\nContent-type: text/html\n\n", stdout);
		fputs("<h1>Authorization received</h1>", stdout);
		if (useragent)
		{
			printf("<p>%s\n", useragent);
		}
		printf("<p>%s\n", auth);

		char *p = strstr(auth, "Basic ");
		if (p)
		{
			const char base64[] =
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz"
				"0123456789+/=";
			int b64buf = 0, b64count = 0, c;

			fputs("<p>Decoded authentication: ", stdout);

			for (p += 6; *p; p ++)
			{
				char *b = strchr(base64, *p);
				int b64val = b ? (b - base64) : 0;
				if (64 == b64val) b64val = 0;
				b64buf = (b64buf << 6) | b64val;
				switch (++ b64count)
				{
					case 2:
						c = (b64buf & 0xff0) >> 4;
						fputsafe(c);
						b64buf &= 0x0f;
						break;

					case 3:
						c = (b64buf & 0x3fc) >> 2;
						fputsafe(c);
						b64buf &= 0x03;
						break;

					case 4:
						c = b64buf & 0xff;
						fputsafe(c);
						b64buf = 0;
						b64count = 0;
						break;
				}
			}
		}
	}

	return 0;
}

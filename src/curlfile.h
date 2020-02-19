#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <curl/curl.h>

void curlFile(char URL[]) {
	CURLcode ret;
	CURL *hnd;

	hnd = curl_easy_init ();
	curl_easy_setopt (hnd, CURLOPT_URL, URL);
	curl_easy_setopt (hnd, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt (hnd, CURLOPT_USERAGENT, "curl/7.35.0");
	curl_easy_setopt (hnd, CURLOPT_MAXREDIRS, 50L);
	curl_easy_setopt (hnd, CURLOPT_TCP_KEEPALIVE, 1L);

	ret = curl_easy_perform (hnd);

	curl_easy_cleanup (hnd);
	hnd = NULL;

	if (ret != 0) {
		char msg[100];
		strcat(msg, "curl ");
		strcat(msg, URL);
		errno = ret;
		perror(msg);
		exit(EXIT_FAILURE);
	}
}

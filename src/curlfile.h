#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <errno.h>

#define CURL_STATICLIB

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written;
	written = fwrite(ptr, size, nmemb, stream);
	return written;
}

void curlFile(char *url, char filename[256]) {
    CURL *pCurl;
    FILE *fptr;
    CURLcode codes;
    int returnCode;

    pCurl = curl_easy_init();

    if (pCurl) {
        fptr = fopen(filename,"wb");
        curl_easy_setopt(pCurl, CURLOPT_URL, url);
        curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(pCurl, CURLOPT_WRITEDATA, fptr);
        returnCode = curl_easy_perform(pCurl);
        curl_easy_cleanup(pCurl);
        fclose(fptr);

	if (returnCode != 0) {
		errno = returnCode;
		perror("curl");
		exit(EXIT_FAILURE);
	}
    }
}

#pragma once
#ifndef SIMPLE_CURL_HPP__
#define SIMPLE_CURL_HPP__

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

#include <curl/curl.h>

/**
 * A deliberately oversimplified class for making HTTP requests with libcurl.
 * All requests are blocking.
 */
namespace lib {
namespace simple_curl {

struct InMemoryResponse {
  long status_code;
  std::string response;
  std::vector<std::string> headers;
};

class EasyHandle {
  public:
    static const int MAX_REDIRECTS = 10;
    EasyHandle(long timeout_secs)
      : curl_handle_(curl_easy_init()) {
      // Set up book-keeping.
      curl_easy_setopt(curl_handle_, CURLOPT_NOSIGNAL, 1);
      curl_easy_setopt(curl_handle_, CURLOPT_NOPROGRESS, 1);
      curl_easy_setopt(curl_handle_, CURLOPT_ERRORBUFFER, &error_buffer);
      curl_easy_setopt(curl_handle_, CURLOPT_FOLLOWLOCATION, 1);
      curl_easy_setopt(curl_handle_, CURLOPT_MAXREDIRS, MAX_REDIRECTS);
      curl_easy_setopt(curl_handle_, CURLOPT_TIMEOUT, timeout_secs);
      // Handle response body
      curl_easy_setopt(curl_handle_, CURLOPT_WRITEDATA, &response_);
      curl_easy_setopt(curl_handle_, CURLOPT_WRITEFUNCTION, &EasyHandle::WriteResponse);
      // Handle response headers
      curl_easy_setopt(curl_handle_, CURLOPT_HEADERDATA, &response_);
      curl_easy_setopt(curl_handle_, CURLOPT_HEADERFUNCTION, &EasyHandle::WriteHeader);
    }

    ~EasyHandle() { if (curl_handle_) curl_easy_cleanup(curl_handle_); }
    EasyHandle(const EasyHandle& e) = delete;
    EasyHandle& operator=(const EasyHandle& e) = delete;
    EasyHandle(EasyHandle&& e) = default;
    EasyHandle& operator=(EasyHandle&& e) = default;

    long PerformRequest(const std::string& url) {
      // Set the target URL and we're good to go.
      curl_easy_setopt(curl_handle_, CURLOPT_URL, url.c_str());
      auto ret = curl_easy_perform(curl_handle_);
      if (ret != CURLE_OK) {
        return 0;
      }
      return curl_easy_getinfo(curl_handle_, CURLINFO_RESPONSE_CODE, &(response_.status_code)) == CURLE_OK
        ? response_.status_code
        : 0;
    }

    InMemoryResponse& GetResponse() { return response_; }

    std::string GetErrors() { return std::string(error_buffer, strlen(error_buffer)); }

  private:
    static size_t WriteResponse(char* ptr, size_t size, size_t n, void* data_ptr) {
      size_t res = size*n;
      if (res && ptr && data_ptr) {
        InMemoryResponse* mem = (InMemoryResponse*) data_ptr;
        mem->response.append(ptr, res);
        return res;
      }
      return 0;
    }

    static size_t WriteHeader(char* ptr, size_t size, size_t n, void* data_ptr) {
      size_t res = size*n;
      if (res && ptr && data_ptr) {
        InMemoryResponse* mem = (InMemoryResponse*) data_ptr;
        mem->headers.emplace_back(ptr, res);
        return res;
      }
      return 0;
    }

    CURL* curl_handle_;
    InMemoryResponse response_;
    char error_buffer[CURL_ERROR_SIZE];
};

class Global {
  public:
    static const int DEFAULT_TIMEOUT_SECS = 10;
    Global() { curl_global_init(CURL_GLOBAL_ALL); }
    Global(long flags) { curl_global_init(flags); }
    Global(const Global& c) = delete;
    Global& operator=(const Global& c) = delete;
    Global(Global&& c) = delete;
    Global& operator=(Global&& c) = delete;
    ~Global() { curl_global_cleanup(); }

    EasyHandle make_handle() {
      return EasyHandle(DEFAULT_TIMEOUT_SECS);
    }
};

}  // namespace simple_curl
}  // namespace lib

#endif // SIMPLE_CURL_HPP__

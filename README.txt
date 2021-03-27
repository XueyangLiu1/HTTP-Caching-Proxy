Docker:
Please cd docker-depoly/src and run "chmod o+x run.sh" to give authority to run.sh


Cache:
1. Data Structure
  Our cache is build based on linkedlist and map. Our get and put operation is O(1).
  We followed LRU policy
2. Store policy
  Our cache would store responses that are 200 OK and not private and not no-store
3. Expire Calculate
  We have three ways to calculate freshness.
  2.1 If max-age exists, we compare max-age and current age
  2.2 Else if Expires exists, we compare Expire time and current time.
  2.3 Else if Last modify exists, we calculate freshness time by (Date - Last Modify)/10, and then compare freshness time and current age.
  2.4 If none of these exists, we would resend the request.
4. Revalidate
  We would revalidate no-cache respones and stale responses.
  We have two ways to revalidate the response.
  3.1 If ETag exists, we would add If-None-Match header in request and send it.
  3.2 Else if Last-Modify exists, we would add If-Modified-Since header in request and send it.
  3.3 If none of these exists, we would resend the request.
  If returned response is 304, we would use the cached response. Otherwise, we use the new response.

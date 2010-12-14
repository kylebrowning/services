<?php
 class ServicesWebTestCase extends DrupalWebTestCase {
  protected function servicesGet($url, $data = NULL, $parameters = NULL) {
    $options = array();
    $url = $this->getAbsoluteUrl($url);
    $this->pass($url, 'URL');
    $content = $this->curlExec(array(CURLOPT_HTTPGET => TRUE, CURLOPT_URL => url($url, $options), CURLOPT_NOBODY => FALSE, CURLOPT_RETURNTRANSFER => TRUE, CURLOPT_HEADER => TRUE, CURLOPT_HTTPHEADER => array("Accept: application/json")));
    $info = curl_getinfo($this->curlHandle);
    $header = substr($content, 0, $info['header_size']);
    $status = strtok($header, "\r\n");  
    $code = $info['http_code'];
    $body = substr($content, -$info['download_content_length']);  
    $this->verbose('GET request to: ' . $url .
                   '<hr />Ending URL: ' . $this->getUrl() .
                   '<hr />' . $content);
    return array('header' => $header, 'status' => $status, 'code' => $code, 'body' => $body);
  }

}
?>
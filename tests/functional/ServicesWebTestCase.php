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
  protected function servicesPost($url, $data = NULL, $headers = array()) {
    $options = array();
    $url = $this->getAbsoluteUrl($url);
    $this->pass($url, 'URL');
    $content = $this->curlExec(array(CURLOPT_URL => $url, CURLOPT_POST => TRUE, CURLOPT_POSTFIELDS => $data, CURLOPT_HTTPHEADER => $headers, CURLOPT_RETURNTRANSFER => TRUE));
    $info = curl_getinfo($this->curlHandle);
    $header = substr($content, 0, $info['header_size']);
    $status = strtok($header, "\r\n");  
    $code = $info['http_code'];
    $body = substr($content, -$info['download_content_length']);  
    $this->verbose('POST request to: ' . $url .
                    'data: ' . $data .
                   '<hr />Ending URL: ' . $this->getUrl() .
                   '<hr />' . $content);
    return array('header' => $header, 'status' => $status, 'code' => $code, 'body' => $body);
  }
  protected function servicesPut($url, $data = NULL, $headers = array()) {
    $options = array();
    $url = $this->getAbsoluteUrl($url);
    $this->pass($url, 'URL');
    $putData = tmpfile();
    // Write the string to the temporary file
    fwrite($putData, $data);
    fseek($putData, 0);
    $content = $this->curlExec(array(CURLOPT_URL => $url,CURLOPT_RETURNTRANSFER => TRUE, CURLOPT_CUSTOMREQUEST => "PUT", CURLOPT_POSTFIELDS => $data, CURLOPT_HTTPHEADER => $headers, CURLOPT_INFILE =>$putData, CURLOPT_INFILESIZE => strlen($data)));
    $info = curl_getinfo($this->curlHandle);
    $header = substr($content, 0, $info['header_size']);
    $status = strtok($header, "\r\n");  
    $code = $info['http_code'];
    $body = substr($content, -$info['download_content_length']);  
    $this->verbose('PUT request to: ' . $url .
                    'data: ' . $data .
                   '<hr />Ending URL: ' . $this->getUrl() .
                   '<hr />' . $content);
    fclose($putData);
    return array('header' => $header, 'status' => $status, 'code' => $code, 'body' => $body);
  }
  protected function servicesDelete($url, $data = NULL, $headers = array()) {
    $options = array();
    $url = $this->getAbsoluteUrl($url);
    $this->pass($url, 'URL');
    $content = $this->curlExec(array(CURLOPT_URL => $url, CURLOPT_CUSTOMREQUEST => "DELETE", CURLOPT_HTTPHEADER => $headers, CURLOPT_RETURNTRANSFER => TRUE));
    $info = curl_getinfo($this->curlHandle);
    $header = substr($content, 0, $info['header_size']);
    $status = strtok($header, "\r\n");  
    $code = $info['http_code'];
    $body = substr($content, -$info['download_content_length']);  
    $this->verbose('DELETE request to: ' . $url .
                    'data: ' . $data .
                   '<hr />Ending URL: ' . $this->getUrl() .
                   '<hr />' . $content);
    return array('header' => $header, 'status' => $status, 'code' => $code, 'body' => $body);
  }
}
?>
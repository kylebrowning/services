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

   /*
    ------------------------------------
    HELPER METHODS
    ------------------------------------
   */

   /**
    * Creates a data array for populating an endpoint creation form.
    *
    * @return
    * An array of fields for fully populating an endpoint creation form.
    */
   public function populateEndpointFAPI() {
     return array(
       'name'   => 'mchnname',
       'title'  => $this->randomName(20),
       'path'   => $this->randomName(10),
       'server' => 'rest_server',
     );
   }
   public function saveNewEndpoint() {
    $edit = $this->populateEndpointFAPI() ;
    $endpoint = new stdClass;
    $endpoint->disabled = FALSE; /* Edit this to true to make a default endpoint disabled initially */
    $endpoint->api_version = 3;
    $endpoint->name = $edit['name'];
    $endpoint->title = $edit['title'];
    $endpoint->server = $edit['server'];
    $endpoint->path = $edit['path'];
    $endpoint->authentication = array(
      'services_sessauth' => array(),
    );
    $endpoint->resources = array(
      'node' => array(
        'alias' => '',
        'operations' => array(
          'create' => array(
            'enabled' => 1,
          ),
          'retrieve' => array(
            'enabled' => 1,
          ),
          'update' => array(
            'enabled' => 1,
          ),
          'delete' => array(
            'enabled' => 1,
          ),
          'index' => array(
            'enabled' => 1,
          ),
        ),
      ),
      'system' => array(
        'alias' => '',
        'actions' => array(
          'connect' => array(
            'enabled' => 1,
          ),
          'get_variable' => array(
            'enabled' => 1,
          ),
          'set_variable' => array(
            'enabled' => 1,
          ),
        ),
      ),
      'taxonomy_term' => array(
        'alias' => '',
        'operations' => array(
          'create' => array(
            'enabled' => 1,
          ),
          'retrieve' => array(
            'enabled' => 1,
          ),
          'update' => array(
            'enabled' => 1,
          ),
          'delete' => array(
            'enabled' => 1,
          ),
        ),
        'actions' => array(
          'selectNodes' => array(
            'enabled' => 1,
          ),
        ),
      ),
      'taxonomy_vocabulary' => array(
        'alias' => '',
        'operations' => array(
          'create' => array(
            'enabled' => 1,
          ),
          'retrieve' => array(
            'enabled' => 1,
          ),
          'update' => array(
            'enabled' => 1,
          ),
          'delete' => array(
            'enabled' => 1,
          ),
        ),
        'actions' => array(
          'getTree' => array(
            'enabled' => 1,
          ),
        ),
      ),
      'user' => array(
        'alias' => '',
        'operations' => array(
          'create' => array(
            'enabled' => 1,
          ),
          'retrieve' => array(
            'enabled' => 1,
          ),
          'update' => array(
            'enabled' => 1,
          ),
          'delete' => array(
            'enabled' => 1,
          ),
          'index' => array(
            'enabled' => 1,
          ),
        ),
        'actions' => array(
          'login' => array(
            'enabled' => 1,
          ),
          'logout' => array(
            'enabled' => 1,
          ),
        ),
      ),
      'comment' => array(
        'alias' => '',
        'operations' => array(
          'create' => array(
            'enabled' => 1,
          ),
          'retrieve' => array(
            'enabled' => 1,
          ),
          'update' => array(
            'enabled' => 1,
          ),
          'delete' => array(
            'enabled' => 1,
          ),
        ),
        'actions' => array(
          'loadNodeComments' => array(
            'enabled' => 1,
          ),
          'countAll' => array(
            'enabled' => 1,
          ),
          'countNew' => array(
            'enabled' => 1,
          ),
        ),
      ),
      'file' => array(
        'alias' => '',
        'operations' => array(
          'create' => array(
            'enabled' => 1,
          ),
          'retrieve' => array(
            'enabled' => 1,
          ),
          'delete' => array(
            'enabled' => 1,
          ),
        ),
        'actions' => array(
          'nodeFiles' => array(
            'enabled' => 1,
          ),
        ),
      ),
      'echo' => array(
        'alias' => '',
        'operations' => array(
          'index' => array(
            'enabled' => 1,
          ),
        ),
      ),
    );
    $endpoint->debug = 1;
    $endpoint->status = 1;
    services_endpoint_save($endpoint);
    $endpoint = services_endpoint_load($endpoint->name);
    if($endpoint->name == $edit['name']) {
      $this->pass('Endpoint successfully created');  
    } else {
       $this->fail('Endpoint creation failed');  
    }
     $this->servicesGet($endpoint->path);
     return $endpoint;
   }
 /**
 * Builds out our post fields
 *
 */
  public function services_build_postfields($data = array()) {
    $post_data = '';  
    if (is_array($data) && !empty($data)) {
      array_walk($data, array($this, 'services_flatten_fields'));
      $post_data = implode('&', $data);
    }
    if(is_object($data) && !empty($data)) {
      array_walk(get_object_vars($data), array($this, 'services_flatten_fields'));
      $post_data = implode('&', $data);
    }
    return $post_data;
  }
/**
 * Modifies our array data so we can turn it into a querystring
 * 
 * @param string $item - array value
 * @param string $key  - key of the array element
 */
  public function services_flatten_fields(&$item, $key) {
    $item = $key .'='. $item;
  }

}
?>
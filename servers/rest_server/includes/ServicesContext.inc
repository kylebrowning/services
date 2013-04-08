<?php

/**
 * ServicesContext object is for storing all global variables. The idea is to prevent
 * RESTServer from accessing global variables directly and get them only from Context.
 */

interface ServicesContextInterface {
  public function buildFromGlobals();
  public function getEndpointPath();
  public function getCanonicalPath();
}

class ServicesContext implements ServicesContextInterface {
  /**
   * Main storage of all data.
   *
   * @var array
   */
  protected $data = array();

  function __construct($endpoint_path) {
    $this->data['endpoint_path'] = $endpoint_path;
  }

  public function buildFromGlobals() {
    $this->data['get'] = $_GET;
    $this->data['server'] = $_SERVER;
    $this->data['post'] = '';
    if ($this->isRequestHasPostBody()) {
      $this->data['post'] = $this->getRawPostData();
    }
  }

  public function getEndpointPath() {
    return $this->data['endpoint_path'];
  }

  public function getCanonicalPath() {
    if (!isset($this->data['canonical_path'])) {
      $endpoint_path = $this->getEndpointPath();
      $endpoint_path_len = drupal_strlen($endpoint_path . '/');
      $this->data['canonical_path'] = drupal_substr($this->data['get']['q'], $endpoint_path_len);
    }

    return $this->data['canonical_path'];
  }

  /**
   * Determine Request Method of the request.
   *
   * @return string
   *   Name of request method (i.e. GET, POST, PUT ...).
   */
  protected function getRequestMethod() {
    if (!isset($this->data['request_method'])) {
      $this->data['request_method'] = $this->getRequestMethodFromGlobals();
    }

    return $this->data['request_method'];
  }

  protected function getRequestMethodFromGlobals() {
    $server = &$this->data['server'];
    $get = &$this->data['get'];

    $method = $server['REQUEST_METHOD'];
    if ($method == 'POST' && isset($server['HTTP_X_HTTP_METHOD_OVERRIDE'])) {
      $method = $server['HTTP_X_HTTP_METHOD_OVERRIDE'];
    }
    if ($method == 'POST' && (isset($get['_method']) && $get['_method'])) {
      $method = $get['_method'];
    }
    if (isset($get['_method'])) {
      unset($get['_method']);
    }

    return $method;
  }

  protected function isRequestHasPostBody() {
    $method = $this->getRequestMethod();
    return (in_array($method, array('POST', 'PUT')));
  }

  protected function getRawPostData() {
    $handle = fopen('drupal://input', 'r');
    $content = '';
    while (!feof($handle)) {
      $content .= fread($handle, 8192);
    }
    fclose($handle);
    return $content;
  }
}
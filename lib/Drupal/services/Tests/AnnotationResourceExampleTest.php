<?php

namespace Drupal\services\Tests;

use Drupal\rest\Tests\RESTTestBase;

/**
 * Tests the Watchdog resource to retrieve log messages.
 */
class AnnotationResourceExampleTest extends RESTTestBase {

  /**
   * Modules to enable.
   *
   * @var array
   */
  public static $modules = array('hal', 'rest', 'services');

  public static function getInfo() {
    return array(
      'name' => 'Annotation Example resource',
      'description' => 'Tests the annotation resource.',
      'group' => 'Services',
    );
  }

  public function setUp() {
    parent::setUp();
    $this->enableService('annotationExample');
  }

  /**
   * Tests rest calls.
   */
  public function testRESTCalls() {
    // Create a user account that has the required permissions to read
    // the watchdog resource via the REST API.
    $account = $this->drupalCreateUser();
    $this->drupalLogin($account);

    $response = $this->httpRequest('exampleGetCall', 'GET', NULL, $this->defaultMimeType);
    $this->assertResponse(200);
    $this->assertHeader('content-type', $this->defaultMimeType);
    $decoded_response = drupal_json_decode($response);
    $decoded_expected = array('message' => 'Hello World!');
    $this->assertIdentical($decoded_expected, $decoded_response, 'exampleGetCall response expected');

    $response = $this->httpRequest('examplePostCall', 'POST', NULL, $this->defaultMimeType);
    $decoded_response = drupal_json_decode($response);
    $decoded_expected = array('message' => array('POST call'));
    $this->assertIdentical($decoded_expected, $decoded_response, 'examplePostCall response expected');

    $arg1 = $this->randomName();
    $arg2 = $this->randomName();
    $response = $this->httpRequest("getCallArguments/$arg1/$arg2", 'GET', NULL, $this->defaultMimeType);
    $decoded_response = drupal_json_decode($response);
    $decoded_expected = array('arg1' => $arg1, 'arg2' => $arg2);
    $this->assertIdentical($decoded_expected, $decoded_response, 'getCallArguments response expected');

    $body = json_encode($decoded_expected);
    $response = $this->httpRequest("postCallArguments", 'POST', $body, $this->defaultMimeType);
    $decoded_response = drupal_json_decode($response);
    $this->assertIdentical($decoded_response, $decoded_expected, 'postCallArguments response is correct');

    $arg3 = $this->randomName();
    $arg4 = $this->randomName();
    $body = json_encode(array('body_arg1' => $arg1, 'body_arg2' => $arg2));
    $response = $this->httpRequest("postCallMixedArguments/$arg3/$arg4", 'POST', $body, $this->defaultMimeType);
    $decoded_response = drupal_json_decode($response);
    $decoded_expected = array('body_arg1' => $arg1, 'body_arg2' => $arg2, 'uri_arg1' => $arg3, 'uri_arg2' => $arg4);
    $this->assertIdentical($decoded_expected, $decoded_response, 'postCallMixedArguments response expected');
  }
}

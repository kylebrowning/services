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
    $decoded_expected = array('message' => 'POST call');

    $this->assertIdentical($decoded_expected, $decoded_response, 'examplePostCall response expected');
  }
}

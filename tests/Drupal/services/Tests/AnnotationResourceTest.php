<?php

namespace Drupal\services\Tests;

use Drupal\Tests\UnitTestCase;
use Drupal\services\Tests\TestAnnotationResource;

class AnnotationResourceTest extends UnitTestCase {
  public static function getInfo() {
    return array(
      'name' => 'Annotation Resource test',
      'description' => 'Validate proper work of Annotation Resource.',
      'group' => 'Services',
    );
  }

  public function setUp() {
    parent::setUp();

    // For contrib module we should add path for autoloader. Only core modules paths gets added by default.
    $loader = require DRUPAL_ROOT . "/core/vendor/autoload.php";
    $services_path = __DIR__ . '/../../../..';
    $loader->add('Drupal\\services', array($services_path . '/lib', $services_path . '/tests'));
  }

  /**
   * Test annotations of the method.
   */
  function testAnnotationReader() {
    $resource = new TestAnnotationResource(array(), 'plugin_id', array());
    $annotated_methods = $resource->getAnnotatedMethods();

    $method_name = 'customCall';
    $this->assertEquals(array($method_name), $annotated_methods, 'Annotated methods retrieved');

    $method_annotation = $resource->getMethodAnnotation($method_name);
    $method_annotation_expected = array('httpMethod' => 'POST', 'uri' => $method_name);
    $this->assertEquals($method_annotation, $method_annotation_expected, 'Annotation for method ' . $method_name . ' retrieved');
  }

  /**
   * Test case for routes() method.
   */
  function testRoutes() {
    $resource = new TestAnnotationResource(array(), 'plugin_id', array());

    $collection = $resource->routes();
    $customCall_route = $collection->get('plugin_id.customcall');
    $path = $customCall_route->getPath();
    $this->assertEquals('/plugin_id/customCall', $path, 'Route path expected');

    $route_methods = $customCall_route->getMethods();
    $httpMethod = reset($route_methods);
    $this->assertEquals('POST', $httpMethod, 'Route method expected');

    $operation = $customCall_route->getRequirement('_operation');
    $this->assertEquals('customCall', $operation, 'Operation expected');
  }
}
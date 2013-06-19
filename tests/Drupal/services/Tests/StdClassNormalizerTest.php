<?php

namespace Drupal\services\Tests;

use Drupal\services\Normalizer\StdClassNormalizer;
use Symfony\Component\Serializer\Serializer;
use Drupal\Tests\UnitTestCase;

/**
 * Test scenario for StdClassNormalizer.
 */
class StdClassNormalizerTest extends UnitTestCase {
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

  public function testStdClassNormalizer() {
    $normalizers = array(
      new StdClassNormalizer(),
    );
    $encoders = array();
    $serializer = new Serializer($normalizers, $encoders);

    $object = (object) array('foo' => 'bar', 'array' => array(1, 'foo' => (object) array('foo' => 'bar')));
    $normalized_object = $serializer->normalize($object);
    $this->assertEquals($object, $normalized_object, 'Normalizer accepted stdClass object.');
  }

}
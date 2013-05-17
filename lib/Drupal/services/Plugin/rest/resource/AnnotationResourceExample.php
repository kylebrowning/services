<?php

namespace Drupal\services\Plugin\rest\resource;

//*   class = Drupal\services\Plugin\rest\resource\AnnotationResourceExample
use Drupal\Component\Annotation\Plugin;
use Drupal\Core\Annotation\Translation;
use Drupal\rest\Plugin\ResourceBase;
use Drupal\rest\ResourceResponse;
use Drupal\services\Plugin\AnnotationResourceBase;
use Drupal\services\Annotation\ResourceMethod;

/**
 * Provides a resource that uses Annotations.
 *
 * @Plugin(
 *   id = "annotationExample",
 *   label = @Translation("Annotation driven resource")
 * )
 */
class AnnotationResourceExample extends AnnotationResourceBase {

  /**
   * Example GET call.
   *
   * @ResourceMethod(
   *   httpMethod = "GET",
   *   uri = "exampleGetCall"
   * )
   */
  public function exampleGetCall() {
    return new ResourceResponse(array('message' => 'Hello World!'));
  }

  /**
   * Example GET call.
   *
   * @ResourceMethod(
   *   httpMethod = "POST",
   *   uri = "examplePostCall"
   * )
   */
  public function examplePostCall() {
    return new ResourceResponse(array('message' => 'POST call'));
  }
}
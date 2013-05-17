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
   * GET call with several parameters in URL.
   *
   * @ResourceMethod(
   *   httpMethod = "GET",
   *   uri = "getCallArguments/{arg1}/{arg2}",
   *   parameters = {
   *     "arg1" = {
   *       "location" = "uri",
   *       "description" = "First argument of the call",
   *     },
   *     "arg2" = {
   *        "location" = "uri",
   *        "description" = "Second argument of the call",
   *     }
   *   }
   * )
   */
  public function getCallArguments($arguments) {
    return new ResourceResponse($arguments);
  }

  /**
   * Example POST call.
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
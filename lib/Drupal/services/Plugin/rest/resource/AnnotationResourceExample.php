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
   * POST call with several parameters in uri and body.
   *
   * @ResourceMethod(
   *   httpMethod = "POST",
   *   uri = "postCallMixedArguments/{uri_arg1}/{uri_arg2}",
   *   parameters = {
   *     "body_arg1" = {
   *       "location" = "body",
   *       "description" = "First body argument of the call",
   *     },
   *     "body_arg2" = {
   *       "location" = "body",
   *       "description" = "Second body argument of the call",
   *     },
   *     "uri_arg1" = {
   *       "location" = "uri",
   *       "description" = "First uri argument of the call",
   *     },
   *     "uri_arg2" = {
   *       "location" = "uri",
   *       "description" = "Second uri argument of the call",
   *     }
   *   }
   * )
   */
  public function postCallMixedArguments($arguments) {
    return new ResourceResponse($arguments);
  }

  /**
   * POST call with several parameters in body.
   *
   * @ResourceMethod(
   *   httpMethod = "POST",
   *   uri = "postCallArguments",
   *   parameters = {
   *     "arg1" = {
   *       "location" = "body",
   *       "description" = "First argument of the call",
   *     },
   *     "arg2" = {
   *        "location" = "body",
   *        "description" = "Second argument of the call",
   *     }
   *   }
   * )
   */
  public function postCallArguments($arguments) {
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
    return new ResourceResponse(array('message' => (object) array('POST call')));
  }
}
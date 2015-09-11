<?php
/**
 * @file
 * Contains \Drupal\services\Plugin\ServiceResponse\Json.php
 */

namespace Drupal\services\Plugin\ServiceResponse;


use Drupal\services\ServiceResponseBase;
use Symfony\Component\HttpFoundation\JsonResponse;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\Serializer\SerializerInterface;

/**
 * @ServiceResponse(
 *   id = "json",
 *   label = @Translation("JSON Response")
 * )
 */
class Json extends ServiceResponseBase {

  public function respond($data, Request $request, SerializerInterface $serializer) {
    return new JsonResponse($data);
  }

}

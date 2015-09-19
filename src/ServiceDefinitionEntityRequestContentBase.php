<?php
/**
 * @file
 * Contains
 */

namespace Drupal\services;


use Drupal\Core\Routing\RouteMatchInterface;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\Serializer\SerializerInterface;

class ServiceDefinitionEntityRequestContentBase extends ServiceDefinitionBase {
  /**
   * {@inheritdoc}
   *
   * @return \Drupal\Core\Entity\EntityInterface|array
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match, SerializerInterface $serializer) {
    // Unserialize the content of the request if there is any.
    $content = $request->getContent();
    if (!empty($content)) {
      return $serializer->deserialize($content, '\Drupal\node\Entity\Node', $request->getContentType(), ['entity_type' => 'node']);
    }
    return [];
  }

}
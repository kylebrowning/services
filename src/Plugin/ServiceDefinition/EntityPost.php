<?php
/**
 * @file
 * Contains \Drupal\services\Plugin\ServiceDefinition\EntityPost.php
 */

namespace Drupal\services\Plugin\ServiceDefinition;


use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\services\ServiceDefinitionBase;
use Drupal\services\ServiceDefinitionEntityRequestContentBase;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\Serializer\SerializerInterface;

/**
 * @ServiceDefinition(
 *   id = "entity_post",
 *   methods = {
 *     "POST"
 *   },
 *   translatable = true,
 *   deriver = "\Drupal\services\Plugin\Deriver\EntityPost"
 * )
 *
 */
class EntityPost extends ServiceDefinitionEntityRequestContentBase {

  /**
   * {@inheritdoc}
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match, SerializerInterface $serializer) {
    $entity = parent::processRequest($request, $route_match, $serializer);
    if ($entity) {
      $entity->save();
      return [$entity->getEntityType()->id() => $entity->id()];
    }
    /**
     * @todo let's return some sort of failure. Probably need to dig into
     * response handling for errors in D8
     */
    return ['hey'];
  }

}

<?php
/**
 * @file
 * Contains \Drupal\services\Plugin\ServiceDefinition\NodeGet.php
 */

namespace Drupal\services\Plugin\ServiceDefinition;


use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\services\ServiceDefinitionBase;
use Drupal\services\ServiceDefinitionEntityRequestContentBase;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\Serializer\SerializerInterface;

/**
 * @ServiceDefinition(
 *   id = "node_create",
 *   title = @Translation("Node: Create"),
 *   description = @Translation("Creates a node object."),
 *   path = "node",
 *   methods = {
 *     "POST"
 *   },
 *   translatable = true,
 *   category = @Translation("Node")
 * )
 *
 */
class NodeCreate extends ServiceDefinitionEntityRequestContentBase {

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

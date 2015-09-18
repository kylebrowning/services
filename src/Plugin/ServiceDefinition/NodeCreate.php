<?php
/**
 * @file
 * Contains \Drupal\services\Plugin\ServiceDefinition\NodeGet.php
 */

namespace Drupal\services\Plugin\ServiceDefinition;


use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\services\ServiceDefinitionBase;
use Symfony\Component\HttpFoundation\Request;

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
 *   category = @Translation("Node"),
 *   context = {
 *     "node" = @ContextDefinition("entity:node", label = @Translation("Node"))
 *   }
 * )
 *
 */
class NodeCreate extends ServiceDefinitionBase {

  /**
   * {@inheritdoc}
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match) {
    /** @var $node \Drupal\node\Entity\Node */
    return ['hey'];
  }

}

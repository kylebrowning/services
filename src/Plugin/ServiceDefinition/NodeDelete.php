<?php
/**
 * @file
 * Contains \Drupal\services\Plugin\ServiceDefinition\NodeGet.php
 */

namespace Drupal\services\Plugin\ServiceDefinition;


use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\services\ServiceDefinitionBase;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\Serializer\SerializerInterface;

/**
 * @ServiceDefinition(
 *   id = "node_delete",
 *   title = @Translation("Node: Delete"),
 *   description = @Translation("Deletes a node object."),
 *   path = "node/{node}",
 *   methods = {
 *     "DELETE"
 *   },
 *   translatable = true,
 *   category = @Translation("Node"),
 *   context = {
 *     "node" = @ContextDefinition("entity:node", label = @Translation("Node"))
 *   }
 * )
 *
 */
class NodeDelete extends ServiceDefinitionBase {

  /**
   * {@inheritdoc}
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match, SerializerInterface $serializer) {
    /** @var $node \Drupal\node\Entity\Node */
    $node = $this->getContextValue('node');
    $node->delete();
  }

}

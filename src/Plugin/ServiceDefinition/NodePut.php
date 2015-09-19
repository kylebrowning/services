<?php
/**
 * @file
 * Contains \Drupal\services\Plugin\ServiceDefinition\NodePut.php
 */

namespace Drupal\services\Plugin\ServiceDefinition;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\services\ServiceDefinitionEntityRequestContentBase;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\Serializer\SerializerInterface;

/**
 * @ServiceDefinition(
 *   id = "node_put",
 *   title = @Translation("Node: Update"),
 *   description = @Translation("Updates a node object."),
 *   path = "node/{node}",
 *   methods = {
 *     "PUT"
 *   },
 *   translatable = true,
 *   category = @Translation("Node"),
 *   context = {
 *     "node" = @ContextDefinition("entity:node", label = @Translation("Node"))
 *   }
 * )
 *
 */
class NodePut extends ServiceDefinitionEntityRequestContentBase {
  /**
   * {@inheritdoc}
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match, SerializerInterface $serializer) {
    $updated_entity = parent::processRequest($request, $route_match, $serializer);
    $entity = $this->getContextValue('node');
    foreach ($updated_entity as $field_name => $field) {
      $entity->set($field_name, $field->getValue());
    }
    $entity->save();
    return $entity->toArray();
  }

}

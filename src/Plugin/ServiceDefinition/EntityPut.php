<?php
/**
 * @file
 * Contains \Drupal\services\Plugin\ServiceDefinition\NodePut.php
 */

namespace Drupal\services\Plugin\ServiceDefinition;
use Drupal\Core\Entity\ContentEntityInterface;
use Drupal\Core\Routing\RouteMatchInterface;
use Drupal\services\ServiceDefinitionEntityRequestContentBase;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\Serializer\SerializerInterface;

/**
 * @ServiceDefinition(
 *   id = "node_put",
 *   methods = {
 *     "PUT"
 *   },
 *   translatable = true,
 *   deriver = "\Drupal\services\Plugin\Deriver\EntityPut"
 * )
 *
 */
class EntityPut extends ServiceDefinitionEntityRequestContentBase {
  /**
   * {@inheritdoc}
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match, SerializerInterface $serializer) {
    $updated_entity = parent::processRequest($request, $route_match, $serializer);
    $entity = $this->getContextValue('entity');
    foreach ($updated_entity as $field_name => $field) {
      if ($entity instanceof ContentEntityInterface) {
        $entity->set($field_name, $field->getValue());
      }
      else {
        $entity->set($field_name, $field);
      }
    }
    $entity->save();
    return $entity->toArray();
  }

}

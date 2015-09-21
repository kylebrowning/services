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
 *   id = "entity_put",
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
    if ($updated_entity) {
      /** @var $entity \Drupal\Core\Entity\EntityInterface */
      $entity = $this->getContextValue('entity');
      if ($entity instanceof ContentEntityInterface) {
        foreach ($updated_entity as $field_name => $field) {
          $entity->set($field_name, $field->getValue());
        }
      }
      else {
        /** @var $updated_entity \Drupal\Core\Config\Entity\ConfigEntityInterface */
        foreach ($updated_entity->toArray() as $field_name => $field) {
          $entity->set($field_name, $field);
        }
      }
      $entity->save();
      return $entity->toArray();
    }
    // @todo throw a proper exception.
    return [];
  }


}

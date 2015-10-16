<?php
/**
 * @file
 * Contains \Drupal\services\Plugin\Deriver\EntityGet.php
 */

namespace Drupal\services\Plugin\Deriver;


use Drupal\Core\Plugin\Context\ContextDefinition;
use Drupal\ctools\Plugin\Deriver\EntityDeriverBase;

class EntityView extends EntityDeriverBase {
  public function getDerivativeDefinitions($base_plugin_definition) {
    foreach ($this->entityManager->getDefinitions() as $entity_type_id => $entity_type) {
      if ($entity_type->hasViewBuilderClass()) {
        $this->derivatives[$entity_type_id] = $base_plugin_definition;
        $this->derivatives[$entity_type_id]['title'] = $this->t('@label: View', ['@label' => $entity_type->getLabel()]);
        $this->derivatives[$entity_type_id]['description'] = $this->t('Renders a @entity_type_id object and serializes it as a response to the current request.', ['@entity_type_id' => $entity_type_id]);
        $this->derivatives[$entity_type_id]['category'] = $this->t('@label', ['@label' => $entity_type->getLabel()]);
        $this->derivatives[$entity_type_id]['path'] = "$entity_type_id/{{$entity_type_id}}/view";
        $this->derivatives[$entity_type_id]['context'] = [
          "$entity_type_id" => new ContextDefinition("entity:$entity_type_id", $this->t('@label', ['@label' => $entity_type->getLabel()]))
        ];
      }
    }
    return $this->derivatives;
  }
}
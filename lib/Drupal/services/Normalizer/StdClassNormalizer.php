<?php

/**
 * @file
 * Contains \Drupal\services\Normalizer\StdClassNormalizer.
 */

namespace Drupal\services\Normalizer;

use Drupal\serialization\Normalizer\NormalizerBase;

/**
 * Normalizer for object of stdClass.
 */
class StdClassNormalizer extends NormalizerBase {

  /**
   * {@inheritdoc}
   */
  protected $supportedInterfaceOrClass = 'stdClass';

  /**
   * {@inheritdoc}
   */
  public function normalize($object, $format = NULL, array $context = array()) {
    $attributes = array();
    foreach ($object as $field_name => $field_item) {
      $attributes[$field_name] = $this->serializer->normalize($field_item, $format);
    }
    return (object) $attributes;
  }

}
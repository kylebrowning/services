<?php

/**
 * @file
 * Contains Drupal\services\ServiceDefinition\TestServiceDefinition.
 */


namespace Drupal\services\Plugin\ServiceDefinition;

use Drupal\Component\Utility\SafeMarkup;
use Drupal\services\Annotation\ServiceDefinition;
use Drupal\services\ServiceDefinitionBase;
use Symfony\Component\HttpFoundation\Request;

/**
 * Enforces a number of a type of character in passwords.
 *
 * @ServiceDefinition(
 *   id = "test_service_definition",
 *   title = @Translation("Testing Service Definition"),
 *   description = @Translation("Provided to test basic service provider definition."),
 *   translatable = true,
 * )
 */
class TestServiceDefinition extends ServiceDefinitionBase {

  /**
   * {@inheritdoc}
   */
  public function getArguments() {
    return ['method', 'uri'];
  }

  /**
   * Testing hello world style request.
   */
  public function processRequest(Request $request) {
    return SafeMarkup::escape($request->getMethod() . ' - ' . $request->getUri());
  }
}

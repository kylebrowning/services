<?php
/**
 * @file
 * Provides Drupal\services\ServiceDefinitionInterface.
 */
namespace Drupal\services;

use Drupal\Component\Plugin\ConfigurablePluginInterface;
use Drupal\Component\Plugin\PluginInspectionInterface;
use Drupal\Component\Serialization\SerializationInterface;
use Drupal\Core\Plugin\PluginFormInterface;
use Symfony\Component\HttpFoundation\Request;

interface ServiceDefinitionInterface extends PluginInspectionInterface {

  /**
   * Returns a translated string for the constraint title.
   * @return string
   */
  public function getTitle();

  /**
   * Returns a translated description for the constraint description.
   * @return string
   */
  public function getDescription();

  /**
   * Returns an array of service request arguments.
   * @return array
   */
  public function getArguments();

  /**
   * Returns a boolean if this service definition supports translations.
   * @return boolean
   */
  public function supportsTranslation();

  /**
   * Returns a true/false status as to if the password meets the requirements of the constraint.
   *
   * @param request
   *   A request object.
   *
   * @return SerializationInterface
   *   The response.
   */
  public function processRequest(Request $request);

}

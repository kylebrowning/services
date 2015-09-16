<?php
/**
 * @file
 * Provides Drupal\services\ServiceDefinitionInterface.
 */
namespace Drupal\services;

use Drupal\Core\Cache\CacheableDependencyInterface;
use Drupal\Core\Plugin\ContextAwarePluginInterface;
use Drupal\Core\Routing\RouteMatchInterface;
use Symfony\Component\HttpFoundation\Request;

interface ServiceDefinitionInterface extends ContextAwarePluginInterface, CacheableDependencyInterface {

  /**
   * Returns a translated string for the service title.
   * @return string
   */
  public function getTitle();

  /**
   * Returns a translated string for the category.
   * @return string
   */
  public function getCategory();

  /**
   * Returns the appended path for the service.
   * @return string
   */
  public function getPath();

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
   * @param \Symfony\Component\HttpFoundation\Request $request
   * @param \Drupal\Core\Routing\RouteMatchInterface $route_match
   *
   * @return array
   *   The response.
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match);

}

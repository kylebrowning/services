<?php
/**
 * @file
 * Contains \Drupal\services\ServiceResponseManager.php
 */

namespace Drupal\services;

use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;
use Drupal\Core\Plugin\DefaultPluginManager;

class ServiceResponseManager extends DefaultPluginManager {

  /**
   * @var \Symfony\Component\Serializer\SerializerInterface
   */
  protected $serializer;

  /**
   * @param \Traversable $namespaces
   * @param \Drupal\Core\Cache\CacheBackendInterface $cache_backend
   * @param \Drupal\Core\Extension\ModuleHandlerInterface $module_handler
   */
  public function __construct(\Traversable $namespaces, CacheBackendInterface $cache_backend, ModuleHandlerInterface $module_handler) {
    parent::__construct('Plugin/ServiceResponse', $namespaces, $module_handler, 'Drupal\services\ServiceResponseInterface', 'Drupal\services\Annotation\ServiceResponse');
    $this->alterInfo('service_response_info');
    $this->setCacheBackend($cache_backend, 'service_response');
  }

  /**
   * {@inheritdoc}
   */
  public function getInstance(array $options) {
    /** @var $request \Symfony\Component\HttpFoundation\Request */
    $request = $options['request'];
    $accept = $request->headers->get('Accept');
    $format = $request->getFormat($accept);
    // Plugins should be named identically to the format supported by the
    // serializer service.
    return $this->createInstance($format);
  }


}

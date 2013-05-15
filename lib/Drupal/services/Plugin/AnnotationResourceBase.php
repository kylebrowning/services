<?php

/**
 * @file
 * Definition of Drupal\rest\Plugin\ResourceBase.
 */

namespace Drupal\services\Plugin;

use Drupal\rest\Plugin\ResourceBase;
use Symfony\Component\Routing\Route;
use Symfony\Component\Routing\RouteCollection;
use Doctrine\Common\Annotations\AnnotationReader;
use Doctrine\Common\Annotations\AnnotationRegistry;

/**
 * Common base class for resource plugins.
 */
abstract class AnnotationResourceBase extends ResourceBase {

  /**
   * Method annotations.
   *
   * @var array
   */
  protected $annotations = array();

  /**
   * @var AnnotationReader.
   */
  protected $reader;

  public function __construct(array $configuration, $plugin_id, array $plugin_definition) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    $this->reader = new AnnotationReader();
  }

  /**
   * Implements ResourceInterface::routes().
   */
  public function routes() {
    $collection = new RouteCollection();
    $path_prefix = strtr($this->pluginId, ':', '/');
    $route_name = strtr($this->pluginId, ':', '.');

    $methods = $this->availableMethods();
    foreach ($methods as $method) {
      $lower_method = strtolower($method);
      $route = new Route("/$path_prefix/{id}", array(
        '_controller' => 'Drupal\rest\RequestHandler::handle',
        // Pass the resource plugin ID along as default property.
        '_plugin' => $this->pluginId,
      ), array(
        // The HTTP method is a requirement for this route.
        '_method' => $method,
        '_permission' => "restful $lower_method $this->pluginId",
      ));

      switch ($method) {
        case 'POST':
          // POST routes do not require an ID in the URL path.
          $route->setPattern("/$path_prefix");
          $route->addDefaults(array('id' => NULL));
          $collection->add("$route_name.$method", $route);
          break;

        case 'GET':
        case 'HEAD':
          // Restrict GET and HEAD requests to the media type specified in the
          // HTTP Accept headers.
          $formats = drupal_container()->getParameter('serializer.formats');
          foreach ($formats as $format_name) {
            // Expose one route per available format.
            //$format_route = new Route($route->getPattern(), $route->getDefaults(), $route->getRequirements());
            $format_route = clone $route;
            $format_route->addRequirements(array('_format' => $format_name));
            $collection->add("$route_name.$method.$format_name", $format_route);
          }
          break;

        default:
          $collection->add("$route_name.$method", $route);
          break;
      }
    }

    return $collection;
  }

  /**
   * We will use annotated methods for building routes.
   */
  public function getAnnotatedMethods() {
    $methods = $this->getMethodNames();

    $this->registerAnnotationAutoloaderNamespace();

    foreach ($methods as $key => $method_name) {
      $annotation_definition = $this->getResourceMethodAnnotationDefinition($method_name);

      if (!empty($annotation_definition )) {
        $this->annotations[$method_name] = $annotation_definition;
      }
      else {
        unset($methods[$key]);
      }
    }

    return $methods;
  }

  /**
   * Retrieve annotation for specified method.
   *
   * @param string $method_name
   *
   * @return array
   */
  public function getMethodAnnotation($method_name) {
    return isset($this->annotations[$method_name]) ? $this->annotations[$method_name] : NULL;
  }

  protected function getMethodNames() {
    $reflection = new \ReflectionClass(get_class($this));

    $methods = array();
    foreach ($reflection->getMethods() as $reflection_method) {
      $methods[] = $reflection_method->name;
    }

    return $methods;
  }

  protected function registerAnnotationAutoloaderNamespace() {
    $annotation_namespaces = array(
      'Drupal\services\Annotation' => DRUPAL_ROOT . '/modules/services/lib',
    );
    AnnotationRegistry::registerAutoloadNamespaces($annotation_namespaces);
  }

  protected function getResourceMethodAnnotationDefinition($method_name) {
    $reflection_method = new \ReflectionMethod(get_class($this), $method_name);
    $annotation = $this->reader->getMethodAnnotation($reflection_method, 'Drupal\services\Annotation\ResourceMethod');
    if (!empty($annotation)) {
      return $annotation->get();
    }
  }
}

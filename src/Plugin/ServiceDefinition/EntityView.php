<?php
/**
 * @file
 * Contains \Drupal\services\Plugin\ServiceDefinition\EntityView.php
 */

namespace Drupal\services\Plugin\ServiceDefinition;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\Core\Render\RendererInterface;
use Drupal\services\ServiceDefinitionBase;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\HttpFoundation\Request;
use Drupal\Core\Routing\RouteMatchInterface;
use Symfony\Component\Serializer\SerializerInterface;

/**
 * @ServiceDefinition(
 *   id = "entity_view",
 *   methods = {
 *     "GET"
 *   },
 *   translatable = true,
 *   deriver = "\Drupal\services\Plugin\Deriver\EntityView"
 * )
 *
 */
class EntityView extends ServiceDefinitionBase implements ContainerFactoryPluginInterface {

  /**
   * @var RendererInterface
   */
  protected $renderer;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static($configuration, $plugin_id, $plugin_definition, $container->get('renderer'));
  }

  /**
   * @param array $configuration
   * @param string $plugin_id
   * @param mixed $plugin_definition
   * @param RendererInterface $renderer
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, RendererInterface $renderer) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    $this->renderer = $renderer;
  }


  /**
   * {@inheritdoc}
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match, SerializerInterface $serializer) {
    $view_mode = 'full';
    if ($request->query->has('view_mode')) {
      $view_mode = $request->query->get('view_mode');
    }
    /** @var $entity \Drupal\Core\Entity\EntityInterface */
    $entity = $this->getContextValue('entity');
    $view_builder = \Drupal::entityManager()->getViewBuilder($entity->getEntityTypeId());
    $elements = $view_builder->view($entity, $view_mode);
    return [
      'render_array' => $elements,
      'data' => $this->renderer->renderRoot($elements)
    ];
  }

}

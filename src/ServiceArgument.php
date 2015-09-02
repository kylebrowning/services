<?php
/**
 * @file
 * Provides Drupal\services\ServiceArgument.
 */

namespace Drupal\services;
use Symfony\Component\HttpFoundation\Request;

class ServiceArgument implements ServiceArgumentInterface {

  /**
   * The argument ID.
   *
   * @var string
   */
  public $id;

  /**
   * The human-readable name of the service argument.
   *
   * @ingroup plugin_translatable
   *
   * @var \Drupal\Core\Annotation\Translation
   */
  public $title;

  /**
   * The service argument is required or not.
   *
   * @var boolean
   */
  public $required;

  /**
   * The message if the argument fails.
   *
   * @ingroup plugin_translatable
   *
   * @var \Drupal\Core\Annotation\Translation
   */
  public $error_message;

  /**
   * The raw argument value from the request.
   *
   * @var mixed
   */
  public $value;

  /**
   * {@inheritdoc}
   */
  public function processArgument(Request $request) {
    if ($request->get($this->id)) {
      $this->value = $request->get($this->id);
      return TRUE;
    }
    return FALSE;
  }
}
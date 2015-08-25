<?php

/**
 * @file
 * Contains Drupal\services\Controller\Services.
 */

namespace Drupal\services\Controller;

use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Routing\RouteMatchInterface;
use Symfony\Component\HttpFoundation\Request;

/**
 * Class Services.
 *
 * @package Drupal\services\Controller
 */
class Services extends ControllerBase {
  /**
   * Processing the API request
   */
  public function processRequest(Request $request, RouteMatchInterface $route_match) {
    //TODO - Process request, match to service API via endpoint, process request.
  }
}

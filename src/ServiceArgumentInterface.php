<?php
/**
 * @file
 * Provides Drupal\services\ServiceArgumentInterface.
 */

namespace Drupal\services;
use Symfony\Component\HttpFoundation\Request;

interface ServiceArgumentInterface {


  /**
   * Processes the request for the argument.
   *
   * @param Symfony\Component\HttpFoundation\Request
   *   A request object.
   *
   * @return boolean
   *   Whether or not the argument was properly represented in the request.
   */
  public function processArgument(Request $request);
}
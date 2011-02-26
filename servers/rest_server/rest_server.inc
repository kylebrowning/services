<?php

/**
 * @file
 * Autoload classes.
 */

function _rest_server_autoload_info() {
  return array(
    'RESTServer' => array(
      'file' => 'includes/RESTServer.inc',
    ),
    'RESTServerViewBuiltIn' => array(
      'file' => 'includes/rest_server.views.inc',
    ),
    'RESTServerView' => array(
      'file' => 'includes/rest_server.views.inc',
    ),
    'RssFormatView' => array(
      'file' => 'formats/RssFormatView.inc',
    ),
    'ResourceFeedModel' => array(
      'file' => 'includes/models.inc',
    ),
    'ResourceFeedModelItem' => array(
      'file' => 'includes/models.inc',
    ),
    'ResourceTimeFeedModel' => array(
      'file' => 'includes/models.inc',
    ),
    'ResourceTimeFeedModelItem' => array(
      'file' => 'includes/models.inc',
    ),
    'NodeResourceFeedModel' => array(
      'file' => 'includes/node_resource.models.inc',
    ),
    'NodeResourceViewFeedModel' => array(
      'file' => 'includes/node_resource.models.inc',
    ),
  );
}
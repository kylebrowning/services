<?php
// $Id$

function _system_resource_definition() {
  return array(
    'system' => array(
      'actions' => array(
        'connect' => array(
          'file' => array('type' => 'inc', 'module' => 'services', 'name' => 'resources/system_resource'),
          'callback' => '_system_resource_connect',
        ),
      ),
    ),
  );
}

function _system_resource_connect() {
  global $user;

  $return = new stdClass();
  $return->sessid = session_id();
  $return->user = $user;

  return $return;
}


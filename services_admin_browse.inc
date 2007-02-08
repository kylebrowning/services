<?php
// $Id$

/**
 * Callback for 'services/browse'
 */
function services_admin_browse_index() {
  $methods = services_get_all();
  
  // show enable server modules
  $output.= '<h2>' . t('Servers') . '</h2>';
  $output.= '<ul>';
  foreach (module_implements('server_info') as $module) {
    $info = module_invoke($module, 'server_info');
    $name = $info['#name'];
    $path = 'services/' . $info['#path'];
    $output .= '<li class="leaf">' . l($name . " - /$path", $path) . '</li>';
  }
  $output.= '</ul>';
  
  $output.= '<h2>' . t('Services') . '</h2>';
  
  // group namespaces
  $services = array();
  foreach ($methods as $method) {
    $namespace = substr($method['#method'], 0, strrpos($method['#method'], '.'));
    $services[$namespace][] = $method;
  }
  
  if (count($services)) {
    foreach ($services as $namespace => $methods) {
      $output.= '<h3>' . $namespace . '</h3>';
      $output.= '<ul>';
      foreach ($methods as $method) {
        $output.= '<li class="leaf">' . l($method['#method'], 'admin/build/services/browse/' . $method['#method']) . '</li>';
      }
      $output.= '</ul>';
    }
  }
  else {
    $output.= t('No services have been enabled.');
  }
  
  return $output;
}

function services_admin_browse_method() {
  global $services_admin_browse_test_submit_result;
  
  if ($_SERVER['HTTP_X_REQUESTED_WITH'] == 'XMLHttpRequest' && user_access('test methods')) {
    drupal_get_form('services_admin_browse_test');
    $output.= '<h3>' . t('Result') . '</h3>';
    $output.= '<code>' . $services_admin_browse_test_submit_result . '</code>';
    print $output;
    exit; 
  }
  
  $method = services_method_get(arg(4));
  
  if (!$method) {
    return $output;
  }
  
  //die(print_r($method, true));
  //drupal_set_title($method['#method']);
  $output.= '<h3>' . $method['#method'] . '</h3>';
  $output.= '<p>' . $method['#help'] . '</p>';
  
  // List arguments
  $output.= '<h3>' . t('Arguments') . ' (' . count($method['#args']) . ')' . '</h3>';
  $output.= '<dl id="service-browser-arguments">';
  $count = 0;
  foreach ($method['#args'] as $arg) {
    $count++;
    $output.= '<dt><em class="type">' . $arg['#type'] . '</em> <strong class="name">' . $arg['#name'] . '</strong> ' . ' (' . (($arg['#optional']) ? t('optional') : t('required')) . ')' . '</dt>';
    $output.= '<dd>' . $arg['#description'] . '</dd>';
  }
  
  $output.= '</dl>';
  
  // allow testing of methods
  $output.= '<h3>' . t('Call method') . '</h3>';
  $output.= drupal_get_form('services_admin_browse_test');
  
  
  
  // display results
  if (user_access('test methods')) {
    $output.= '<div id="output">';
    if ($services_admin_browse_test_submit_result) {
      $output.= '<h3>' . t('Result') . '</h3>';
      $output.= '<code>' . $services_admin_browse_test_submit_result . '</code>';
    }
    $output.= '</div>';
  }
  

  return $output;
}

function services_admin_browse_test() {
  $form = array();
  $method = services_method_get(arg(4));
  
  $form['arg'] = array('#tree' => true);
  
  foreach ($method['#args'] as $key => $arg) {
    $form['name'][$key]         = array('#value' => $arg['#name']);
    $form['optional'][$key]     = array('#value' => ($arg['#optional']) ? t('optional') : t('required'));
    //$form['type'][$key]         = array('#value' => $arg['#type']);
    
    switch ($arg['#name']) {
      case 'api_key':
        $form['arg'][$key] = array('#type' => 'textfield', '#default_value' => services_admin_browse_get_first_key());
        break;
      
      case 'sessid':
        $form['arg'][$key] = array('#type' => 'textfield', '#default_value' => session_id());
        break;
        
      default:
        $form['arg'][$key] = array('#type' => 'textfield');
        break;
    }
    
  }
  $form['submit'] = array('#type' => 'submit', '#value' => t('Call method'));
  
  $form['#redirect'] = false;
  return $form;
}

function services_admin_browse_get_first_key() {
  $keys = services_get_keys();
  foreach ($keys as $kid => $key) {
    return $kid;
  }
}

function services_admin_browse_test_submit($form_id, $form_values) {
  global $services_admin_browse_test_submit_result;
  $method = services_method_get(arg(4));
  $args = services_admin_browse_test_unserialize_args($form_values['arg']);
  $result = services_method_call($method['#method'], $args);
  $services_admin_browse_test_submit_result = check_markup(print_r($result, true), 1);
}

function services_admin_browse_test_unserialize_args($values) {
  $method = services_method_get(arg(4));
  
  // convert args
  for ($c = count($method['#args']) - 1; $c >= 0; $c--) {
    $arg = $method['#args'][$c];
    $value = $values[$c];
    
    // remove empty values from end of array
    // once we find a value, we can no longer skip
    if (empty($value) && !$noskip) continue;
    $noskip = true;
    
    switch ($arg['#type']) {
      case 'array' :
        if (empty($value)) {
          $return[$c] = null;
        }
        else {
          $return[$c] = explode(',', $value);
        }
        
        break;
      default :
        $return[$c] = $value;
    }
  }
  
  if ($return) ksort($return);
  
  //die(print_r($return, true));
  
  return $return;
}

function theme_services_admin_browse_test($form) {
  //die(print_r($form, true));
  $output = '';
  $output.= drupal_render($form['test']);

  $header = array(t('Name'), t('Required'), t('Value'));
  $rows = array();
  foreach (element_children($form['name']) as $key => $type) {
    $row = array();
    $row[] = drupal_render($form['name'][$key]);
    $row[] = drupal_render($form['optional'][$key]);
    $row[] = drupal_render($form['arg'][$key]);
    $rows[] = $row;
  }
  
  $output.= theme('table', $header, $rows);
  $output.= drupal_render($form['submit']);
  
  $output.= drupal_render($form);
  
  return $output;
}
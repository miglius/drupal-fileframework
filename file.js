// $Id$

/**
 * Show popup menus.
 */
Drupal.behaviors.file = function() {
  $('span.file.with-menu span.label').hover(function(event) { 
    $(this).find('.highlight').show();   
  },  
  function(event) { 
    $(this).find('.highlight').hide();   
  }); 
  $('span.file.with-menu span.label').unbind('click').click(function(event) { 
    $(this).toggleClass('active'); 
    $(this).find('ul').toggle('fast'); 
  });
  $(document).click(function(event) {
    $('span.file.with-menu .label').each(function() {
      if ($(event.target).parents('.label')[0] != this) {
        $(this).removeClass('active');
        $(this).find('ul').hide();
      }
    });
  });
};


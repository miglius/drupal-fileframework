// $Id$

Drupal.behaviors.file_gallery = function(context) {
  $('#file-gallery-filter-form').submit(function () {
    return false;
  });

  $('#edit-filter-submit').click(function() {
    var filter = 0;
    $('#file-gallery-filter-form').find('.form-checkbox').each(function(i){
      if($(this).attr('checked')) filter += Math.pow(2,i);
    });
    var url = window.location.href.replace(/([file_gallery|gallery]\/[\d]+\/[\d]+)(\/{0,1}[\w]*\/{0,1})/, '$1/' + filter);
    window.location.href = url;
  });

  $('select#file-gallery-selector').change(function() {
    var filter = this.options[this.selectedIndex].value;
    var url = window.location.href.replace(/([file_gallery|gallery]\/[\d]+\/[\d]+)(\/{0,1}[\w]*\/{0,1})/, filter ? '$1/' + filter : '$1');
    window.location.href = url;
  });
};


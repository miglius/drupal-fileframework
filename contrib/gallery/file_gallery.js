// $Id$

$(document).ready(function() {
  $('select#file-gallery-selector').change(function() {
    var filter = this.options[this.selectedIndex].value;
    // FIXME: This is a hack and potentially fragile.
    var url = window.location.href.replace(/(file_gallery\/[\d]+\/[\d]+)(\/{0,1}[\w]*\/{0,1})/, filter ? '$1/' + filter : '$1');
    window.location.href = url;
  });
});

function findOpenSearchUrl()
{
    head = document.getElementsByTagName('head');

    var result = new Array();

    if (!head) {
        return result;
    }

    var links = head[0].getElementsByTagName('link');

    var j = 0;

    for (var i = 0; i < links.length; i++) {
        var link = links[i];

        if (link.getAttribute('type') == 'application/opensearchdescription+xml' && link.getAttribute('href')) {
            var searchLink = new Array(2);

            searchLink[0] = link.getAttribute('href');

            if (link.getAttribute('title')) {
                searchLink[1] = link.getAttribute('title');
            } else {
                searchLink[1] = '';
            }

            result[j] = searchLink;

            j++;
        }
    }

    return result;
}

var url = findOpenSearchUrl();

url;

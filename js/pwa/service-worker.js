const VERSION = "1_0";
const CACHE_NAME = `pwatest-${VERSION}`;

const INITIAL_CACHED_RESOURCES = [
  "./",
  "./index.html",
  "./app.js"
];

// Add a cache-busting query string to the pre-cached resources.
// This is to avoid loading these resources from the disk cache.
const INITIAL_CACHED_RESOURCES_WITH_VERSIONS = INITIAL_CACHED_RESOURCES.map(path => {
  return `${path}?v=${VERSION}`;
});

self.addEventListener("install", event => {
  self.skipWaiting();

  event.waitUntil((async () => {
    const cache = await caches.open(CACHE_NAME);
    cache.addAll(INITIAL_CACHED_RESOURCES_WITH_VERSIONS);
  })());
});

self.addEventListener("activate", event => {
  event.waitUntil((async () => {
    const names = await caches.keys();
    await Promise.all(names.map(name => {
      if (name !== CACHE_NAME) {
        return caches.delete(name);
      }
    }));
    await clients.claim();
  })());
});

self.addEventListener("fetch", event => {
  const url = new URL(event.request.url);

  if (url.origin !== location.origin) {
    return;
  }

  if (event.request.method !== 'GET') {
    return;
  }

  if (url.pathname.includes("/widgets/")) {
    return;
  }

  // On fetch, go to the cache first, and then network.
  event.respondWith((async () => {
    const cache = await caches.open(CACHE_NAME);
	var u = event.request.url;
    var versionedUrl;
	if (u.includes("?"))
		versionedUrl = u+"&v="+VERSION;
	else
		versionedUrl = u+"?v="+VERSION;
    const cachedResponse = await cache.match(versionedUrl);

    if (cachedResponse) {
      return cachedResponse;
    } else {
      const fetchResponse = await fetch(versionedUrl);
      cache.put(versionedUrl, fetchResponse.clone());
      return fetchResponse;
    }
  })());
});

self.addEventListener('fetch', event => {
  const url = new URL(event.request.url);

  if (event.request.method !== 'POST' || !url.pathname.includes('/handle-shared-song')) {
    return;
  }

/*  event.respondWith(Response.redirect('./'));

  event.waitUntil(async function () {
    const data = await event.request.formData();
    const files = data.getAll('audioFiles');

    // Store the song in a special IDB place for the front-end to pick up later
    // when it starts.
    // Instead of importing idb-keyval here, we just have a few lines of manual
    // IDB code, to store the file in the same keyval store that idb-keyval uses.
    const openReq = indexedDB.open('keyval-store');
    openReq.onupgradeneeded = e => {
      const { target: { result: db } } = e;
      db.createObjectStore("keyval");
    }
    openReq.onsuccess = e => {
      const { target: { result: db } } = e;
      const transaction = db.transaction("keyval", "readwrite");
      const store = transaction.objectStore("keyval");
      store.put(files, 'handle-shared-files');
    }
  }());*/
});

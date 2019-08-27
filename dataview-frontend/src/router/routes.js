const routes = [
    {
        path: "/",
        component: () => import("layouts/MyLayout.vue"),
        children: [{ path: "", component: () => import("pages/Index.vue") }]
    },
    {
        path: "/raw",
        component: () => import("layouts/MyLayout.vue"),
        children: [{ path: "", component: () => import("pages/Table.vue") }]
    },
    {
        path: "/graphs",
        component: () => import("layouts/MyLayout.vue"),
        children: [{ path: "", component: () => import("pages/Graphs.vue") }]
    }
];

// Always leave this as last one
if (process.env.MODE !== "ssr") {
    routes.push({
        path: "*",
        component: () => import("pages/Error404.vue")
    });
}

export default routes;

(function () {
    async function loadSyslogConfig(app) {
        try {
            const cfg = await window.MiOpenApi.requestJson("/api/syslog");
            app.elements.syslogEnabledInput.checked   = !!cfg.enabled;
            app.elements.syslogEnabledInput.dispatchEvent(new Event('change'));
            app.elements.syslogServerInput.value       = cfg.server    || "";
            app.elements.syslogPortInput.value         = cfg.port      || "514";
            app.elements.syslogFacilityInput.value     = cfg.facility  != null ? cfg.facility : "1";
            app.elements.syslogMinLevelInput.value     = cfg.min_level != null ? String(cfg.min_level) : "7";
            app.elements.syslogIdInput.value           = cfg.id        || "";
        } catch (error) {
            console.error("Error fetching syslog config", error);
        }
    }

    async function updateSyslogConfig(app) {
        try {
            const payload = {
                enabled:   app.elements.syslogEnabledInput.checked,
                server:    app.elements.syslogServerInput.value,
                port:      parseInt(app.elements.syslogPortInput.value, 10) || 514,
                facility:  parseInt(app.elements.syslogFacilityInput.value, 10),
                min_level: parseInt(app.elements.syslogMinLevelInput.value, 10)
            };
            const id = app.elements.syslogIdInput.value.trim();
            if (id) payload.id = id;
            const result = await window.MiOpenApi.postJson("/api/syslog", payload);
            if (!result.success) { showToast(result.message || "Syslog save failed.", "error"); return; }
            showToast(result.message || "Syslog settings saved.", "success");
        } catch (error) {
            showToast("Error saving syslog config: " + error.message, "error");
        }
    }

    function init(app) {
        app.elements.syslogIdInput = document.getElementById("syslog-id");
        app.loadSyslogConfig  = function () { return loadSyslogConfig(app); };
        app.updateSyslogConfig = function () { return updateSyslogConfig(app); };
    }

    window.MiOpenSyslog = { init: init };
})();

(function () {
    var container = document.createElement("div");
    container.id = "toast-container";
    document.body.appendChild(container);

    window.showToast = function (msg, type, duration) {
        var toast = document.createElement("div");
        toast.className = "toast" + (type ? " toast-" + type : "");
        toast.textContent = msg;
        container.appendChild(toast);
        var ms = (duration || 3000);
        setTimeout(function () {
            toast.classList.add("toast-hide");
            setTimeout(function () {
                if (toast.parentNode) toast.parentNode.removeChild(toast);
            }, 300);
        }, ms);
    };
})();

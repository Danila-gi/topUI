interface ProcessData {
    command: string;
    cpu: number;
    mem: number;
    ni: number;
    pid: number;
    pr: number;
    res: number;
    shr: number;
    state: string;
    time: number;
    user: string;
    virt: number;
}

interface ApiResponse {
    data: ProcessData[];
}

class SystemMonitor {
    private refreshInterval: number = 3000;
    private intervalId: number | null = null;
    private tableBody: HTMLElement;
    private timestampElement: HTMLElement;

    constructor() {
        this.tableBody = document.getElementById('table-body')!;
        this.timestampElement = document.getElementById('timestamp')!;
        this.startAutoRefresh();
        this.attachSortHandlers();
    }

    private startAutoRefresh(): void {
        this.refreshData();
        
        this.intervalId = window.setInterval(() => {
            this.refreshData();
        }, this.refreshInterval);
    }

    public async refreshData(): Promise<void> {
        try {
            const response = await fetch('http://localhost:80/get_data');
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            
            const data: ApiResponse = await response.json();
            this.updateTable(data.data);
            this.updateTimestamp();
        } catch (error) {
            console.error('Error fetching data:', error);
            this.showError('Failed to load data. Check if backend is running.');
        }
    }

    private updateTable(processes: ProcessData[]): void {
        if (!processes || processes.length === 0) {
            this.tableBody.innerHTML = '<tr><td colspan="12" class="loading">No data available</td></tr>';
            return;
        }

        const fragment = document.createDocumentFragment();
        
        processes.forEach(proc => {
            const row = document.createElement('tr');
            
            const cpuPercent = (proc.cpu * 100).toFixed(6);
                        
            row.innerHTML = `
                <td>${proc.pid}</td>
                <td>${this.escapeHtml(proc.command || 'unknown')}</td>
                <td class="state-${proc.state}">${proc.state}</td>
                <td>${cpuPercent}%</td>
                <td>${proc.res.toLocaleString()}</td>
                <td>${proc.virt.toLocaleString()}</td>
                <td>${proc.pr}</td>
                <td>${proc.ni}</td>
                <td>${proc.time.toLocaleString()}</td>
            `;
            
            fragment.appendChild(row);
        });
        
        this.tableBody.innerHTML = '';
        this.tableBody.appendChild(fragment);
    }

    private updateTimestamp(): void {
        const now = new Date();
        const formatted = now.toLocaleTimeString('ru-RU', {
            hour: '2-digit',
            minute: '2-digit',
            second: '2-digit'
        });
        this.timestampElement.textContent = formatted;
    }

    private showError(message: string): void {
        this.tableBody.innerHTML = `<tr><td colspan="12" class="loading" style="color: #e74c3c;">${message}</td></tr>`;
    }

    private escapeHtml(str: string): string {
        const div = document.createElement('div');
        div.textContent = str;
        return div.innerHTML;
    }

    private attachSortHandlers(): void {
        const headers = document.querySelectorAll('th');
        headers.forEach((header, index) => {
            header.addEventListener('click', () => {
                this.sortTable(index);
            });
        });
    }

    private sortTable(columnIndex: number): void {
        const rows = Array.from(this.tableBody.querySelectorAll('tr'));
        const isAscending = this.tableBody.getAttribute('data-sort-order') !== 'asc';
        
        rows.sort((a, b) => {
            const aValue = a.children[columnIndex]?.textContent || '';
            const bValue = b.children[columnIndex]?.textContent || '';
            
            const aNum = parseFloat(aValue);
            const bNum = parseFloat(bValue);
            
            if (!isNaN(aNum) && !isNaN(bNum)) {
                return isAscending ? aNum - bNum : bNum - aNum;
            }
            
            return isAscending 
                ? aValue.localeCompare(bValue)
                : bValue.localeCompare(aValue);
        });
        
        rows.forEach(row => this.tableBody.appendChild(row));
        this.tableBody.setAttribute('data-sort-order', isAscending ? 'asc' : 'desc');
    }

    public stopAutoRefresh(): void {
        if (this.intervalId) {
            clearInterval(this.intervalId);
            this.intervalId = null;
        }
    }
}

let monitor: SystemMonitor;

document.addEventListener('DOMContentLoaded', () => {
    monitor = new SystemMonitor();
    
    (window as any).refreshData = () => {
        monitor.refreshData();
    };
});

window.addEventListener('beforeunload', () => {
    if (monitor) {
        monitor.stopAutoRefresh();
    }
});
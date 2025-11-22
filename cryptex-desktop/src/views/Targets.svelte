<script lang="ts">
  import { Plus, Trash2, Edit } from 'lucide-svelte';
  import { onMount } from 'svelte';

  interface Target {
    id: string;
    name: string;
    hosts: string;
    description: string;
    created_at: string;
  }

  let targets: Target[] = [];
  let showAddDialog = false;
  let editingTarget: Target | null = null;
  let newTarget = {
    name: '',
    hosts: '',
    description: ''
  };

  onMount(() => {
    loadTargets();
  });

  function loadTargets() {
    // Load from localStorage for now
    const stored = localStorage.getItem('openfire_targets');
    if (stored) {
      targets = JSON.parse(stored);
    } else {
      // Add some example targets
      targets = [
        {
          id: '1',
          name: 'Production Servers',
          hosts: '192.168.1.0/24',
          description: 'Production network range',
          created_at: new Date().toISOString()
        },
        {
          id: '2',
          name: 'Web Servers',
          hosts: '10.0.1.10-10.0.1.20',
          description: 'Web server cluster',
          created_at: new Date().toISOString()
        }
      ];
      saveTargets();
    }
  }

  function saveTargets() {
    localStorage.setItem('openfire_targets', JSON.stringify(targets));
  }

  function addTarget() {
    if (!newTarget.name || !newTarget.hosts) return;

    const target: Target = {
      id: Date.now().toString(),
      name: newTarget.name,
      hosts: newTarget.hosts,
      description: newTarget.description,
      created_at: new Date().toISOString()
    };

    targets = [...targets, target];
    saveTargets();
    resetForm();
  }

  function deleteTarget(id: string) {
    if (confirm('Are you sure you want to delete this target?')) {
      targets = targets.filter(t => t.id !== id);
      saveTargets();
    }
  }

  function startEdit(target: Target) {
    editingTarget = target;
    newTarget = {
      name: target.name,
      hosts: target.hosts,
      description: target.description
    };
    showAddDialog = true;
  }

  function updateTarget() {
    if (!editingTarget || !newTarget.name || !newTarget.hosts) return;

    targets = targets.map(t =>
      t.id === editingTarget.id
        ? { ...t, ...newTarget }
        : t
    );
    saveTargets();
    resetForm();
  }

  function resetForm() {
    newTarget = { name: '', hosts: '', description: '' };
    editingTarget = null;
    showAddDialog = false;
  }
</script>

<div class="targets-view">
  <div class="page-header">
    <div>
      <h1>Targets</h1>
      <p class="subtitle">Manage scan targets and host lists</p>
    </div>
    <button class="btn btn-primary" on:click={() => showAddDialog = true}>
      <Plus size={18} />
      <span>New Target</span>
    </button>
  </div>

  <div class="targets-list">
    {#if targets.length === 0}
      <div class="empty-state">
        <p>No targets defined</p>
        <button class="btn btn-primary" on:click={() => showAddDialog = true}>
          <Plus size={18} />
          <span>Create First Target</span>
        </button>
      </div>
    {:else}
      <table class="targets-table">
        <thead>
          <tr>
            <th>Name</th>
            <th>Hosts</th>
            <th>Description</th>
            <th>Created</th>
            <th class="actions-col">Actions</th>
          </tr>
        </thead>
        <tbody>
          {#each targets as target}
            <tr>
              <td class="target-name">{target.name}</td>
              <td class="target-hosts"><code>{target.hosts}</code></td>
              <td class="target-description">{target.description || '-'}</td>
              <td>{new Date(target.created_at).toLocaleDateString()}</td>
              <td class="actions-col">
                <button class="icon-btn" on:click={() => startEdit(target)} title="Edit">
                  <Edit size={16} />
                </button>
                <button class="icon-btn danger" on:click={() => deleteTarget(target.id)} title="Delete">
                  <Trash2 size={16} />
                </button>
              </td>
            </tr>
          {/each}
        </tbody>
      </table>
    {/if}
  </div>

  {#if showAddDialog}
    <div class="modal-overlay" on:click={resetForm}>
      <div class="modal" on:click|stopPropagation>
        <div class="modal-header">
          <h2>{editingTarget ? 'Edit Target' : 'New Target'}</h2>
        </div>
        <div class="modal-body">
          <div class="form-group">
            <label for="name">Target Name *</label>
            <input
              type="text"
              id="name"
              bind:value={newTarget.name}
              placeholder="e.g., Production Servers"
            />
          </div>
          <div class="form-group">
            <label for="hosts">Hosts *</label>
            <input
              type="text"
              id="hosts"
              bind:value={newTarget.hosts}
              placeholder="192.168.1.0/24, 10.0.1.10-20, example.com"
            />
            <small>IP ranges, CIDR notation, or hostnames</small>
          </div>
          <div class="form-group">
            <label for="description">Description</label>
            <textarea
              id="description"
              bind:value={newTarget.description}
              placeholder="Optional description"
              rows="3"
            ></textarea>
          </div>
        </div>
        <div class="modal-footer">
          <button class="btn btn-secondary" on:click={resetForm}>Cancel</button>
          <button
            class="btn btn-primary"
            on:click={() => editingTarget ? updateTarget() : addTarget()}
            disabled={!newTarget.name || !newTarget.hosts}
          >
            {editingTarget ? 'Update' : 'Create'}
          </button>
        </div>
      </div>
    </div>
  {/if}
</div>

<style>
  .targets-view {
    padding: 2rem;
    max-width: 1400px;
    margin: 0 auto;
  }

  .page-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 2rem;
  }

  .page-header h1 {
    margin: 0;
    font-size: 2rem;
    color: var(--text-primary);
  }

  .subtitle {
    margin: 0.5rem 0 0 0;
    color: var(--text-muted);
  }

  .targets-list {
    background: var(--bg-secondary);
    border-radius: 8px;
    border: 1px solid var(--border-color);
  }

  .empty-state {
    padding: 4rem 2rem;
    text-align: center;
    color: var(--text-muted);
  }

  .targets-table {
    width: 100%;
    border-collapse: collapse;
  }

  .targets-table th {
    text-align: left;
    padding: 1rem;
    border-bottom: 2px solid var(--border-color);
    color: var(--text-secondary);
    font-weight: 600;
    font-size: 0.875rem;
    text-transform: uppercase;
  }

  .targets-table td {
    padding: 1rem;
    border-bottom: 1px solid var(--border-color);
    color: var(--text-primary);
  }

  .targets-table tbody tr:hover {
    background: var(--hover-bg);
  }

  .target-name {
    font-weight: 500;
  }

  .target-hosts code {
    background: var(--bg-tertiary);
    padding: 0.25rem 0.5rem;
    border-radius: 4px;
    font-size: 0.875rem;
    color: var(--primary-color);
  }

  .actions-col {
    width: 100px;
    text-align: right;
  }

  .icon-btn {
    background: none;
    border: none;
    padding: 0.5rem;
    cursor: pointer;
    color: var(--text-secondary);
    border-radius: 4px;
    transition: all 0.2s;
  }

  .icon-btn:hover {
    background: var(--hover-bg);
    color: var(--primary-color);
  }

  .icon-btn.danger:hover {
    color: var(--critical-color);
  }

  .btn {
    display: inline-flex;
    align-items: center;
    gap: 0.5rem;
    padding: 0.75rem 1.5rem;
    border: none;
    border-radius: 6px;
    font-weight: 500;
    cursor: pointer;
    transition: all 0.2s;
  }

  .btn-primary {
    background: var(--primary-color);
    color: var(--text-primary);
  }

  .btn-primary:hover:not(:disabled) {
    background: var(--primary-hover);
  }

  .btn-primary:disabled {
    opacity: 0.5;
    cursor: not-allowed;
  }

  .btn-secondary {
    background: var(--bg-tertiary);
    color: var(--text-secondary);
  }

  .btn-secondary:hover {
    background: var(--hover-bg);
  }

  /* Modal styles */
  .modal-overlay {
    position: fixed;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background: rgba(0, 0, 0, 0.7);
    display: flex;
    align-items: center;
    justify-content: center;
    z-index: 1000;
  }

  .modal {
    background: var(--bg-secondary);
    border-radius: 8px;
    width: 90%;
    max-width: 600px;
    border: 1px solid var(--border-color);
  }

  .modal-header {
    padding: 1.5rem;
    border-bottom: 1px solid var(--border-color);
  }

  .modal-header h2 {
    margin: 0;
    font-size: 1.5rem;
    color: var(--text-primary);
  }

  .modal-body {
    padding: 1.5rem;
  }

  .modal-footer {
    padding: 1.5rem;
    border-top: 1px solid var(--border-color);
    display: flex;
    justify-content: flex-end;
    gap: 1rem;
  }

  .form-group {
    margin-bottom: 1.5rem;
  }

  .form-group label {
    display: block;
    margin-bottom: 0.5rem;
    color: var(--text-secondary);
    font-weight: 500;
  }

  .form-group input,
  .form-group textarea {
    width: 100%;
    padding: 0.75rem;
    background: var(--bg-tertiary);
    border: 1px solid var(--border-color);
    border-radius: 6px;
    color: var(--text-primary);
    font-family: inherit;
  }

  .form-group small {
    display: block;
    margin-top: 0.25rem;
    color: var(--text-muted);
    font-size: 0.875rem;
  }
</style>
